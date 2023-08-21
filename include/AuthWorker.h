//
// Created by hanyuan on 2023/8/21.
//

#ifndef THULOGIN_AUTHWORKER_H
#define THULOGIN_AUTHWORKER_H
#include <string>

class AuthWorker {
    std::string base_url;
    std::string challenge_api_url;
    std::string portal_api_url;
    std::string ip_addr;
    int ac_id;
    std::string username;
    std::string password;
public:
    AuthWorker(std::string & base_url, int ac_id);
    void set_account(std::string& username,std::string& password);
    int auth();
};


#endif //THULOGIN_AUTHWORKER_H
