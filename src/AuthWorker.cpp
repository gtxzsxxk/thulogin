//
// Created by hanyuan on 2023/8/21.
//

#include "../include/AuthWorker.h"


AuthWorker::AuthWorker(std::string &base_url, int ac_id) {
    if(base_url[base_url.size()-1]=='/'){
        base_url.pop_back();
    }
    this->base_url = base_url;
    this->ac_id = ac_id;
    challenge_api_url = base_url + "/cgi-bin/get_challenge";
    portal_api_url = base_url + "/cgi-bin/srun_portal";
}

void AuthWorker::set_account(std::string& username, std::string& password) {
    this->username = username;
    this->password = password;
}

int AuthWorker::auth() {
    return 0;
}

