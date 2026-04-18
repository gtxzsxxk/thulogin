//
// Created by hanyuan on 2023/8/21.
//
#include <iostream>
#include <sstream>

#include "../lib/Encoder.h"
#include "../lib/json.hpp"
using json = nlohmann::json;
#include "../include/Authenticator.h"
#include "../include/SrunBase64.h"
#include "../include/SrunMd5.h"
#include "../include/SrunSHA1.h"
#include "../include/SrunXEncode.h"
#include "../include/HTTPSClient.h"
#include <regex>

const static std::string banner = "****** thulogin ******\r\n*** Initializing\r\n";

static std::string strip_jsonp(const std::string& data) {
    size_t paren = data.find('(');
    if (paren != std::string::npos && data.back() == ')') {
        return data.substr(paren + 1, data.size() - paren - 2);
    }
    return data;
}

Authenticator::Authenticator(std::string base_url, std::string ac_id,
                             std::string user_agent) :
        base_url(base_url), ac_id(ac_id), user_agent(user_agent) {
    if (!base_url.empty() && base_url.back() == '/') {
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
    std::cout << "*** Authenticate Username: " << username << std::endl;
    if (get_info() != 0) {
        std::cerr << "*** Failed to fetch auth token. Aborting." << std::endl;
        return -1;
    }

    std::string info, hmd5, chksum;
    build_auth_info(info, hmd5, chksum);

    std::stringstream ss;
    Encoder encoder;
    ss << portal_api_url << "?callback=jQuery11240645308969715664_" << getTimestamp() << "&action=login&username="
       << encoder.UrlEncode(username) << "&password={MD5}" << encoder.UrlEncode(hmd5) << "&ac_id="
       << encoder.UrlEncode(ac_id)
       << "&ip=" << encoder.UrlEncode(ip_addr) << "&chksum=" << encoder.UrlEncode(chksum)
       << "&info=" << encoder.UrlEncode(info) << "&n=200&type=1&double_stack=1&_="
       << getTimestamp();
    std::string url_with_params = ss.str();
    try {
        HTTPSClient client;
        auto response = client.get(url_with_params, headers);
        std::string result = strip_jsonp(response);
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

int Authenticator::logout() {
    if (get_info() != 0) {
        std::cerr << "Failed to fetch auth token for logout." << std::endl;
        return -1;
    }

    // Build info JSON without password field
    std::stringstream ss_info;
    ss_info << R"({"username":")" << username << R"(","ip":")" << ip_addr
           << R"(","acid":")" << ac_id << R"(","enc_ver":"srun_bx1"})";
    std::string info = "{SRBX1}" + get_base64_string(x_encode(ss_info.str(), token));

    // Build checksum without hmd5
    std::string checksum = token + username;
    checksum += token + ac_id;
    checksum += token + ip_addr;
    checksum += token + "200";
    checksum += token + "1";
    checksum += token + info;
    checksum = get_sha1(checksum);

    std::stringstream ss;
    Encoder encoder;
    ss << portal_api_url << "?callback=jQuery11240645308969715664_" << getTimestamp()
       << "&action=logout&username=" << encoder.UrlEncode(username)
       << "&ac_id=" << encoder.UrlEncode(ac_id)
       << "&ip=" << encoder.UrlEncode(ip_addr)
       << "&chksum=" << encoder.UrlEncode(checksum)
       << "&info=" << encoder.UrlEncode(info)
       << "&n=200&type=1&double_stack=1&_=" << getTimestamp();

    std::string url_with_params = ss.str();
    try {
        HTTPSClient client;
        auto response = client.get(url_with_params, headers);
        std::string result = strip_jsonp(response);

        if (!fetch_error_state(result)) {
            std::cout << "*** Logged out successfully!" << std::endl;
            return 0;
        } else {
            std::cerr << "*** Failed to log out. The error below occurred:" << std::endl << response_msg << std::endl;
            return -1;
        }
    }
    catch (const std::exception &e) {
        std::cerr << "Failed when trying to logout." << std::endl;
        return -1;
    }
}

bool Authenticator::is_online(std::string& out_username) {
    out_username = "";
    try {
        // Step 1: GET /srun_portal_pc to find IP
        std::stringstream ss;
        ss << base_url << "/srun_portal_pc?ac_id=" << ac_id;
        HTTPSClient client;
        auto response = client.get(ss.str(), headers);

        // Extract IP with regex: ip\s+:\s"([0-9.]+)"
        std::regex ip_regex(R"regex(ip\s+:\s"([0-9.]+)")regex");
        std::smatch match;
        if (!std::regex_search(response, match, ip_regex) || match.size() < 2) {
            return false;
        }
        std::string ip = match[1].str();

        // Step 2: GET /cgi-bin/rad_user_info to check online status
        std::stringstream ss2;
        ss2 << base_url << "/cgi-bin/rad_user_info?ip=" << ip;
        auto info = client.get(ss2.str(), headers);

        auto j = json::parse(strip_jsonp(info));
        if (j.contains("error") && j["error"] == "ok") {
            if (j.contains("user_name")) {
                out_username = j["user_name"].get<std::string>();
            }
            return true;
        }
    } catch (...) {}
    return false;
}

std::string Authenticator::detect_ac_id(bool v6) {
    std::string url = "http://login.tsinghua.edu.cn/index_1.html";
    if (v6) {
        url = "http://mirrors6.tuna.tsinghua.edu.cn/";
    }
    try {
        HTTPSClient client;
        auto response = client.get(url);

        std::regex ac_regex(R"((ac_id=|index_)([0-9]+))");
        std::smatch match;
        if (std::regex_search(response, match, ac_regex) && match.size() >= 3) {
            return match[2].str();
        }
    } catch (...) {}
    return "1";
}

int Authenticator::get_info() {
    try {
        std::stringstream ss;
        ss << challenge_api_url << "?callback=jQuery11240645308969715664_" << getTimestamp()
           << "&username=" << username << "&ip=&double_stack=1&_=" << getTimestamp();
        std::string url_with_params(ss.str());
        HTTPSClient client;
        auto response = client.get(url_with_params, headers);
        std::string result = strip_jsonp(response);
        fetch_ip(result);
        fetch_token(result);
        if (ip_addr.empty() || token.empty()) {
            std::cerr << "Failed to parse IP or token from server response." << std::endl;
            return -1;
        }
        return 0;
    }
    catch (const std::exception &e) {
        std::cerr << "Failed when trying to fetch the auth token." << std::endl;
        return -1;
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
    dest = "";
    try {
        auto j = json::parse(data);
        if (j.contains(prop)) {
            if (j[prop].is_string()) {
                dest = j[prop].get<std::string>();
            } else if (j[prop].is_number()) {
                dest = std::to_string(j[prop].get<int>());
            }
        }
    } catch (const std::exception& e) {
        // parse failed, dest remains empty
    }
}

void Authenticator::fetch_token(const std::string &data) {
    fetch_from_json(data, "challenge", token);
}

void Authenticator::fetch_ip(const std::string &data) {
    fetch_from_json(data, "client_ip", ip_addr);
}

void Authenticator::build_auth_info(std::string &info, std::string &hmd5, std::string &checksum) {
    std::stringstream ss;
    ss << R"({"username":")" << username << R"(","password":")" << password << R"(","ip":")" << ip_addr
       << R"(","acid":")" << ac_id << R"(","enc_ver":"srun_bx1"})";
    info = "{SRBX1}" + get_base64_string(x_encode(ss.str(), token));
    hmd5 = get_md5(password);
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

