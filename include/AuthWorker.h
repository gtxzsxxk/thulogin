//
// Created by hanyuan on 2023/8/21.
//

#ifndef THULOGIN_AUTHWORKER_H
#define THULOGIN_AUTHWORKER_H

#include <string>
#include <ctime>
#include "../lib/HTTPRequest.hpp"

class AuthWorker {
    std::string base_url;
    std::string challenge_api_url;
    std::string portal_api_url;
    std::string user_agent;
    std::string ip_addr;
    std::string ac_id;
    std::string username;
    std::string password;
    std::string token;
    http::HeaderFields headers;

    void get_info();

    void fetch_token(const std::string &data);

    void fetch_ip(const std::string &data);

    void build_auth_info(std::string &info, std::string &hmd5, std::string &checksum);

    static std::time_t getTimestamp();

public:
    AuthWorker(std::string base_url, std::string ac_id,
               std::string user_agent = "Mozilla/5.0 (Windows NT 10.0; Win64; x64)");

    int auth(std::string username, std::string password);
};


#endif //THULOGIN_AUTHWORKER_H
