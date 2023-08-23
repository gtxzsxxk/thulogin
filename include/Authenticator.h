//
// Created by hanyuan on 2023/8/21.
//

#ifndef THULOGIN_AUTHENTICATOR_H
#define THULOGIN_AUTHENTICATOR_H

#include <string>
#include <ctime>
#include "../lib/HTTPRequest.hpp"

class Authenticator {
    std::string base_url;
    std::string challenge_api_url;
    std::string portal_api_url;
    std::string user_agent;
    std::string ip_addr;
    std::string ac_id;
    std::string username;
    std::string password;
    std::string token;
    std::string error_state;
    std::string response_msg;
    http::HeaderFields headers;

    void get_info();

    static void fetch_from_json(const std::string &data, const std::string &prop, std::string &dest);

    void fetch_token(const std::string &data);

    void fetch_ip(const std::string &data);

    int fetch_error_state(const std::string &data);

    void build_auth_info(std::string &info, std::string &hmd5, std::string &checksum);

    static std::time_t getTimestamp();

public:
    Authenticator(std::string base_url, std::string ac_id,
                  std::string user_agent);

    int auth(std::string username, std::string password);
};


#endif //THULOGIN_AUTHENTICATOR_H
