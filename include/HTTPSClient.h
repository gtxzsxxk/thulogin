#ifndef HTTPSCLIENT_H
#define HTTPSCLIENT_H

#include <string>
#include <map>
#include <stdexcept>

class HTTPSClient {
public:
    using Headers = std::map<std::string, std::string>;

    explicit HTTPSClient(const std::string& caFile = "");
    ~HTTPSClient();

    // Perform HTTPS GET request. Returns response body as string.
    // Throws std::runtime_error on failure.
    std::string get(const std::string& url, const Headers& headers = {});

    // Set custom CA certificate file path
    void setCaFile(const std::string& caFile);

    // Parsed URL components
    struct UrlParts {
        std::string scheme;
        std::string host;
        std::string port;
        std::string path;
    };

private:
    std::string caFile_;

    // Parse URL into scheme, host, port, path
    UrlParts parseUrl(const std::string& url);

    // HTTP request (for http:// URLs, no TLS)
    std::string httpGet(const UrlParts& parts, const Headers& headers);

    // HTTPS request (for https:// URLs, with TLS)
    std::string httpsGet(const UrlParts& parts, const Headers& headers);
};

#endif
