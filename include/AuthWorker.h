//
// Created by hanyuan on 2023/8/21.
//

#ifndef THULOGIN_AUTHWORKER_H
#define THULOGIN_AUTHWORKER_H
#include <string>
#include <ctime>

class AuthWorker {
    std::string base_url;
    std::string challenge_api_url;
    std::string portal_api_url;
    std::string ip_addr;
    std::string ac_id;
    std::string username;
    std::string password;
    std::string token;
    void get_info();
    void fetch_token(const std::string & data);
    void fetch_ip(const std::string & data);
    void build_auth_info(std::string & info,std::string & hmd5,std::string & checksum);
    static std::time_t getTimestamp();
public:
    AuthWorker(std::string  base_url, std::string ac_id);
    int auth(std::string username, std::string password);
};


#endif //THULOGIN_AUTHWORKER_H
