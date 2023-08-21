//
// Created by hanyuan on 2023/8/21.
//
#include <iostream>
#include "../include/AuthWorker.h"
#include "../lib/HTTPRequest.hpp"

static const std::string user_agent = "Mozilla/5.0 (Windows NT 10.0; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/63.0.3239.26 Safari/537.36";
static const http::HeaderFields headers={
        std::make_pair("User-Agent",user_agent)
};

AuthWorker::AuthWorker(std::string base_url, int ac_id) {
    if(base_url[base_url.size()-1]=='/'){
        base_url.pop_back();
    }
    this->base_url = base_url;
    this->ac_id = ac_id;
    challenge_api_url = base_url + "/cgi-bin/get_challenge";
    portal_api_url = base_url + "/cgi-bin/srun_portal";
    try{
        http::Request request(base_url+"/srun_portal_pc_ac.php?ac_id=135&");
        const auto response = request.send("GET","",headers);
        std::string result = std::string{response.body.begin(),response.body.end()};
        std::string match_str = "name=\"user_ip\" ";
        size_t index = result.find(match_str);
    }
    catch(const std::exception& e){
        std::cerr<<"Failed when trying to fetch ip address."<<std::endl;
    }
}

void AuthWorker::set_account(std::string& username, std::string& password) {
    this->username = username;
    this->password = password;
}

int AuthWorker::auth() {
    return 0;
}

