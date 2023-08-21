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
    int ac_id;
    std::string username;
    std::string password;
    std::string token;
    void get_info();
    void digest_token(const std::string & data);
    void digest_ip(const std::string & data);
    static std::time_t getTimestamp();
public:
    AuthWorker(std::string  base_url, int ac_id);
    int auth(std::string username, std::string password);
};


#endif //THULOGIN_AUTHWORKER_H
