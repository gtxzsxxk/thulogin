//
// Created by hanyuan on 2023/8/21.
//
#include <iostream>
#include <sstream>

#include "../lib/Encoder.h"
#include "../include/Authenticator.h"
#include "../include/SrunBase64.h"
#include "../include/SrunMd5.h"
#include "../include/SrunSHA1.h"
#include "../include/SrunXEncode.h"

const static std::string banner = "****** thulogin ******\r\n*** Initializing\r\n";

Authenticator::Authenticator(std::string base_url, std::string ac_id,
                             std::string user_agent) :
        base_url(base_url), ac_id(ac_id), user_agent(user_agent) {
    if (base_url[base_url.size() - 1] == '/') {
        base_url.pop_back();
    }
    challenge_api_url = base_url + "/cgi-bin/get_challenge";
    portal_api_url = base_url + "/cgi-bin/srun_portal";
    headers = {
            std::make_pair("User-Agent", user_agent)
    };
    std::cout << banner << std::endl;
    std::cout << "*** Auth Server: " << base_url << std::endl;
    std::cout << "*** Auth Ac_id: " << ac_id << std::endl;
    std::cout << "*** Auth U/A: " << user_agent << std::endl;
}

int Authenticator::auth(std::string username, std::string password) {
    this->username = username;
    this->password = password;

    std::cout << std::endl << "*** Start authenticating..." << std::endl;
    std::cout << std::endl << "*** Authenticate Username: " << username << std::endl;
    get_info();

    std::string info, hmd5, chksum;
    build_auth_info(info, hmd5, chksum);

    std::stringstream ss;
    Encoder encoder;
    ss << portal_api_url << "?callback=jQuery11240645308969715664_" << getTimestamp() << "&action=login&username="
       << encoder.UrlEncode(username) << "&password={MD5}" << encoder.UrlEncode(hmd5) << "&ac_id="
       << encoder.UrlEncode(ac_id)
       << "&ip=" << encoder.UrlEncode(ip_addr) << "&chksum=" << encoder.UrlEncode(chksum)
       << "&info=" << encoder.UrlEncode(info) << "&n=200&type=1&os=windows+10&name=windows&double_stack=1&_="
       << getTimestamp();
    std::string url_with_params = ss.str();
    try {
        http::Request request(url_with_params);
        const auto response = request.send("GET", "", headers);
        std::string result = std::string{response.body.begin(), response.body.end()};
        if (!fetch_error_state(result)) {
            std::cout << "*** Logged in successfully!" << std::endl;
        } else {
            std::cerr << "*** Failed to log in. The error below occurred:" << std::endl << response_msg << std::endl;
        }
    }
    catch (const std::exception &e) {
        std::cerr << "Failed when trying to authenticate." << std::endl;
    }

    return 0;
}

void Authenticator::get_info() {
    try {
        std::stringstream ss;
        ss << challenge_api_url << "?callback=jQuery11240645308969715664_" << getTimestamp()
           << "&username=" << username << "&ip=&_=" << getTimestamp();
        std::string url_with_params(ss.str());
        http::Request request(url_with_params);
        const auto response = request.send("GET", "", headers);
        std::string result = std::string{response.body.begin(), response.body.end()};
        fetch_ip(result);
        fetch_token(result);
    }
    catch (const std::exception &e) {
        std::cerr << "Failed when trying to fetch the auth token." << std::endl;
    }
}

/* milliseconds */
std::time_t Authenticator::getTimestamp() {
    std::chrono::time_point<std::chrono::system_clock, std::chrono::milliseconds> tp = std::chrono::time_point_cast<std::chrono::milliseconds>(
            std::chrono::system_clock::now());
    auto tmp = std::chrono::duration_cast<std::chrono::milliseconds>(tp.time_since_epoch());
    std::time_t timestamp = tmp.count();
    return timestamp;
}

void Authenticator::fetch_from_json(const std::string &data, const std::string &prop, std::string &dest) {
    std::string prop_match = "\"" + prop + "\":";
    size_t prop_ptr = data.find(prop_match);
    prop_ptr += prop_match.size();
    int counter = 0;
    dest = "";
    while (counter < 2) {
        if (data[prop_ptr++] == '"') {
            counter++;
        }
        if (counter > 0 && counter < 2) {
            dest += data[prop_ptr];
        }
    }
    dest.pop_back();
}

void Authenticator::fetch_token(const std::string &data) {
    fetch_from_json(data, "challenge", token);
}

void Authenticator::fetch_ip(const std::string &data) {
    fetch_from_json(data, "client_ip", ip_addr);
}

void Authenticator::build_auth_info(std::string &info, std::string &hmd5, std::string &checksum) {
    //{"username":"asd","password":"asd","ip":"183.172.106.23","acid":"135","enc_ver":"srun_bx1"}
    std::stringstream ss;
    ss << R"({"username":")" << username << R"(","password":")" << password << R"(","ip":")" << ip_addr
       << R"(","acid":")" << ac_id << R"(","enc_ver":"srun_bx1"})";
    info = "{SRBX1}" + get_base64_string(x_encode(ss.str(), token));
    hmd5 = get_hmac_md5(password, token);
    checksum = (token + username);
    checksum += (token + hmd5);
    checksum += (token + ac_id);
    checksum += (token + ip_addr);
    checksum += (token + "200");
    checksum += (token + "1");
    checksum += (token + info);
    checksum = get_sha1(checksum);
}

int Authenticator::fetch_error_state(const std::string &data) {
    fetch_from_json(data, "error", error_state);
    if (error_state == "ok") {
        fetch_from_json(data, "suc_msg", response_msg);
        if (response_msg == "login_ok") {
            return 0;
        }
    } else if (error_state == "login_error") {
        fetch_from_json(data, "error_msg", response_msg);
    } else {
        fetch_from_json(data, "res", response_msg);
    }
    return -1;
}

