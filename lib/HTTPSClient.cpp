#include "HTTPSClient.h"

#include "mbedtls/build_info.h"
#include "mbedtls/ssl.h"
#include "mbedtls/net_sockets.h"
#include "mbedtls/entropy.h"
#include "mbedtls/ctr_drbg.h"
#include "mbedtls/x509_crt.h"
#include "mbedtls/error.h"

#include <string>
#include <vector>
#include <sstream>
#include <stdexcept>
#include <cstring>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <arpa/inet.h>

static std::string mbedtlsError(int err) {
    char buf[256];
    mbedtls_strerror(err, buf, sizeof(buf));
    return std::string(buf);
}

HTTPSClient::HTTPSClient(const std::string& caFile) : caFile_(caFile) {}

HTTPSClient::~HTTPSClient() = default;

std::string HTTPSClient::get(const std::string& url, const Headers& headers) {
    auto parts = parseUrl(url);
    if (parts.scheme == "https") {
        return httpsGet(parts, headers);
    } else if (parts.scheme == "http") {
        return httpGet(parts, headers);
    } else {
        throw std::runtime_error("Unsupported URL scheme: " + parts.scheme);
    }
}

void HTTPSClient::setCaFile(const std::string& caFile) {
    caFile_ = caFile;
}

HTTPSClient::UrlParts HTTPSClient::parseUrl(const std::string& url) {
    UrlParts parts;
    size_t pos = 0;

    // Parse scheme
    size_t schemeEnd = url.find("://", pos);
    if (schemeEnd == std::string::npos) {
        throw std::runtime_error("Invalid URL: missing scheme");
    }
    parts.scheme = url.substr(pos, schemeEnd - pos);
    pos = schemeEnd + 3;

    // Parse host and port
    size_t pathStart = url.find('/', pos);
    std::string hostPort;
    if (pathStart == std::string::npos) {
        hostPort = url.substr(pos);
        parts.path = "/";
    } else {
        hostPort = url.substr(pos, pathStart - pos);
        parts.path = url.substr(pathStart);
    }

    size_t portStart = hostPort.find(':');
    if (portStart == std::string::npos) {
        parts.host = hostPort;
        if (parts.scheme == "https") {
            parts.port = "443";
        } else {
            parts.port = "80";
        }
    } else {
        parts.host = hostPort.substr(0, portStart);
        parts.port = hostPort.substr(portStart + 1);
    }

    return parts;
}

static std::string buildHttpRequest(const std::string& method,
                                    const HTTPSClient::UrlParts& parts,
                                    const HTTPSClient::Headers& headers) {
    std::ostringstream req;
    req << method << " " << parts.path << " HTTP/1.1\r\n";
    req << "Host: " << parts.host << "\r\n";
    req << "Connection: close\r\n";
    for (const auto& [key, value] : headers) {
        req << key << ": " << value << "\r\n";
    }
    req << "\r\n";
    return req.str();
}

static std::string parseHttpResponse(const std::string& response) {
    size_t headerEnd = response.find("\r\n\r\n");
    if (headerEnd == std::string::npos) {
        // Try \n\n fallback
        headerEnd = response.find("\n\n");
        if (headerEnd == std::string::npos) {
            return response; // Return everything as body
        }
        return response.substr(headerEnd + 2);
    }
    return response.substr(headerEnd + 4);
}

std::string HTTPSClient::httpGet(const UrlParts& parts, const Headers& headers) {
    struct addrinfo hints{}, *res = nullptr;
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    int status = getaddrinfo(parts.host.c_str(), parts.port.c_str(), &hints, &res);
    if (status != 0) {
        throw std::runtime_error("getaddrinfo failed for " + parts.host + ": " + gai_strerror(status));
    }

    int sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
    if (sockfd < 0) {
        freeaddrinfo(res);
        throw std::runtime_error("socket creation failed: " + std::string(strerror(errno)));
    }

    if (connect(sockfd, res->ai_addr, res->ai_addrlen) < 0) {
        close(sockfd);
        freeaddrinfo(res);
        throw std::runtime_error("connect failed for " + parts.host + ":" + parts.port + ": " + strerror(errno));
    }
    freeaddrinfo(res);

    std::string request = buildHttpRequest("GET", parts, headers);
    ssize_t sent = send(sockfd, request.c_str(), request.size(), 0);
    if (sent < 0 || static_cast<size_t>(sent) != request.size()) {
        close(sockfd);
        throw std::runtime_error("send failed: " + std::string(strerror(errno)));
    }

    std::string response;
    char buffer[4096];
    while (true) {
        ssize_t n = recv(sockfd, buffer, sizeof(buffer), 0);
        if (n < 0) {
            close(sockfd);
            throw std::runtime_error("recv failed: " + std::string(strerror(errno)));
        }
        if (n == 0) break;
        response.append(buffer, static_cast<size_t>(n));
    }
    close(sockfd);

    return parseHttpResponse(response);
}

std::string HTTPSClient::httpsGet(const UrlParts& parts, const Headers& headers) {
    mbedtls_ssl_context ssl;
    mbedtls_ssl_config conf;
    mbedtls_entropy_context entropy;
    mbedtls_ctr_drbg_context ctrDrbg;
    mbedtls_x509_crt cacert;
    mbedtls_net_context serverFd;

    mbedtls_ssl_init(&ssl);
    mbedtls_ssl_config_init(&conf);
    mbedtls_entropy_init(&entropy);
    mbedtls_ctr_drbg_init(&ctrDrbg);
    mbedtls_x509_crt_init(&cacert);
    mbedtls_net_init(&serverFd);

    auto cleanup = [&]() {
        mbedtls_ssl_free(&ssl);
        mbedtls_ssl_config_free(&conf);
        mbedtls_entropy_free(&entropy);
        mbedtls_ctr_drbg_free(&ctrDrbg);
        mbedtls_x509_crt_free(&cacert);
        mbedtls_net_free(&serverFd);
    };

    const char* pers = "thulogin_https";
    int ret = mbedtls_ctr_drbg_seed(&ctrDrbg, mbedtls_entropy_func, &entropy,
                                    reinterpret_cast<const unsigned char*>(pers),
                                    strlen(pers));
    if (ret != 0) {
        cleanup();
        throw std::runtime_error("mbedtls_ctr_drbg_seed failed: " + mbedtlsError(ret));
    }

    ret = mbedtls_ssl_config_defaults(&conf,
                                      MBEDTLS_SSL_IS_CLIENT,
                                      MBEDTLS_SSL_TRANSPORT_STREAM,
                                      MBEDTLS_SSL_PRESET_DEFAULT);
    if (ret != 0) {
        cleanup();
        throw std::runtime_error("mbedtls_ssl_config_defaults failed: " + mbedtlsError(ret));
    }

    mbedtls_ssl_conf_authmode(&conf, MBEDTLS_SSL_VERIFY_REQUIRED);
    mbedtls_ssl_conf_rng(&conf, mbedtls_ctr_drbg_random, &ctrDrbg);

    // Load CA certificates
    bool caLoaded = false;
    if (!caFile_.empty()) {
        ret = mbedtls_x509_crt_parse_file(&cacert, caFile_.c_str());
        if (ret == 0) {
            caLoaded = true;
        }
    }
    if (!caLoaded) {
        ret = mbedtls_x509_crt_parse_file(&cacert, "/etc/ssl/certs/ca-certificates.crt");
        if (ret == 0) {
            caLoaded = true;
        }
    }
    if (!caLoaded) {
        ret = mbedtls_x509_crt_parse_file(&cacert, "/etc/ssl/cert.pem");
        if (ret == 0) {
            caLoaded = true;
        }
    }
    if (!caLoaded) {
        cleanup();
        throw std::runtime_error("Failed to load CA certificates. Tried: " +
                                 (caFile_.empty() ? "" : caFile_ + ", ") +
                                 "/etc/ssl/certs/ca-certificates.crt, /etc/ssl/cert.pem");
    }
    mbedtls_ssl_conf_ca_chain(&conf, &cacert, nullptr);

    ret = mbedtls_ssl_setup(&ssl, &conf);
    if (ret != 0) {
        cleanup();
        throw std::runtime_error("mbedtls_ssl_setup failed: " + mbedtlsError(ret));
    }

    ret = mbedtls_ssl_set_hostname(&ssl, parts.host.c_str());
    if (ret != 0) {
        cleanup();
        throw std::runtime_error("mbedtls_ssl_set_hostname failed: " + mbedtlsError(ret));
    }

    ret = mbedtls_net_connect(&serverFd, parts.host.c_str(), parts.port.c_str(), MBEDTLS_NET_PROTO_TCP);
    if (ret != 0) {
        cleanup();
        throw std::runtime_error("mbedtls_net_connect failed for " + parts.host + ":" + parts.port +
                                 ": " + mbedtlsError(ret));
    }

    mbedtls_ssl_set_bio(&ssl, &serverFd, mbedtls_net_send, mbedtls_net_recv, nullptr);

    ret = mbedtls_ssl_handshake(&ssl);
    if (ret != 0) {
        cleanup();
        throw std::runtime_error("mbedtls_ssl_handshake failed: " + mbedtlsError(ret));
    }

    uint32_t flags = mbedtls_ssl_get_verify_result(&ssl);
    if (flags != 0) {
        char vrfyBuf[512];
        mbedtls_x509_crt_verify_info(vrfyBuf, sizeof(vrfyBuf), "", flags);
        cleanup();
        throw std::runtime_error("Certificate verification failed: " + std::string(vrfyBuf));
    }

    std::string request = buildHttpRequest("GET", parts, headers);
    size_t written = 0;
    while (written < request.size()) {
        ret = mbedtls_ssl_write(&ssl,
                                reinterpret_cast<const unsigned char*>(request.c_str()) + written,
                                request.size() - written);
        if (ret <= 0) {
            cleanup();
            throw std::runtime_error("mbedtls_ssl_write failed: " + mbedtlsError(ret));
        }
        written += static_cast<size_t>(ret);
    }

    std::string response;
    unsigned char buf[4096];
    while (true) {
        ret = mbedtls_ssl_read(&ssl, buf, sizeof(buf));
        if (ret == MBEDTLS_ERR_SSL_PEER_CLOSE_NOTIFY) {
            break;
        }
        if (ret == 0) {
            break;
        }
        if (ret < 0) {
            cleanup();
            throw std::runtime_error("mbedtls_ssl_read failed: " + mbedtlsError(ret));
        }
        response.append(reinterpret_cast<char*>(buf), static_cast<size_t>(ret));
    }

    cleanup();
    return parseHttpResponse(response);
}
