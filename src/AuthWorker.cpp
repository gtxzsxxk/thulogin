//
// Created by hanyuan on 2023/8/21.
//
#include <iostream>
#include <sstream>
#include "../include/AuthWorker.h"
#include "../lib/HTTPRequest.hpp"
#include "../lib/Encoder.h"
#include "../include/SrunBase64.h"
#include "../include/SrunMd5.h"
#include "../include/SrunSHA1.h"
#include "../include/SrunXEncode.h"

static const std::string user_agent = "Mozilla/5.0 (Windows NT 10.0; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/63.0.3239.26 Safari/537.36";
static const http::HeaderFields headers = {
        std::make_pair("User-Agent", user_agent)
};

AuthWorker::AuthWorker(std::string base_url, std::string ac_id) {
    if (base_url[base_url.size() - 1] == '/') {
        base_url.pop_back();
    }
    this->base_url = base_url;
    this->ac_id = ac_id;
    challenge_api_url = base_url + "/cgi-bin/get_challenge";
    portal_api_url = base_url + "/cgi-bin/srun_portal";
}

int AuthWorker::auth(std::string username, std::string password) {
    this->username = username;
    this->password = password;
    get_info();

    std::string info,hmd5,chksum;
    build_auth_info(info,hmd5,chksum);

    std::stringstream ss;
    Encoder encoder;
    ss<<portal_api_url<<"?callback=jQuery11240645308969715664_"<<getTimestamp()<<"&action=login&username="
    <<encoder.UrlEncode(username)<<"&password={MD5}"<<encoder.UrlEncode(hmd5)<<"&ac_id="<<encoder.UrlEncode(ac_id)
    <<"&ip="<<encoder.UrlEncode(ip_addr)<<"&chksum="<<encoder.UrlEncode(chksum)
    <<"&info="<<encoder.UrlEncode(info)<<"&n=200&type=1&os=windows+10&name=windows&double_stack=1&_="<<getTimestamp();

    std::string url_with_params = ss.str();
    http::Request request(url_with_params);
    const auto response = request.send("GET","",headers);
    std::string result = std::string{response.body.begin(), response.body.end()};
    std::cout<<result<<std::endl;
    return 0;
}

void AuthWorker::get_info() {
    try {
        std::stringstream ss;
        ss<<challenge_api_url<<"?callback=jQuery11240645308969715664_"<<getTimestamp()
        <<"&username="<<username<<"&ip=&_="<<getTimestamp();
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
std::time_t AuthWorker::getTimestamp() {
    std::chrono::time_point<std::chrono::system_clock, std::chrono::milliseconds> tp = std::chrono::time_point_cast<std::chrono::milliseconds>(
            std::chrono::system_clock::now());
    auto tmp = std::chrono::duration_cast<std::chrono::milliseconds>(tp.time_since_epoch());
    std::time_t timestamp = tmp.count();
    return timestamp;
}

void AuthWorker::fetch_token(const std::string &data) {
    std::string token_match = R"("challenge":)";
    size_t ip_ptr = data.find(token_match);
    ip_ptr+=token_match.size();
    int counter = 0;
    while(counter<2){
        if(data[ip_ptr++]=='"'){
            counter++;
        }
        if(counter>0&&counter<2){
            token+=data[ip_ptr];
        }
    }
    token.pop_back();
}

void AuthWorker::fetch_ip(const std::string &data) {
    std::string ip_match = R"("client_ip":)";
    size_t ip_ptr = data.find(ip_match);
    ip_ptr+=ip_match.size();
    int counter = 0;
    while(counter<2){
        if(data[ip_ptr++]=='"'){
            counter++;
        }
        if(counter>0&&counter<2){
            ip_addr+=data[ip_ptr];
        }
    }
    ip_addr.pop_back();
}

void AuthWorker::build_auth_info(std::string & info,std::string & hmd5,std::string & checksum) {
    //{"username":"asd","password":"asd","ip":"183.172.106.23","acid":"135","enc_ver":"srun_bx1"}
    std::stringstream ss;
    ss<<R"({"username":")"<<username<<R"(","password":")"<<password<<R"(","ip":")"<<ip_addr
    <<R"(","acid":")"<<ac_id<<R"(","enc_ver":"srun_bx1"})";
    info = "{SRBX1}"+ get_base64_string(x_encode(ss.str(),token));
    hmd5 = get_hmac_md5(password,token);
    checksum=(token+username);
    checksum+=(token+hmd5);
    checksum+=(token+ac_id);
    checksum+=(token+ip_addr);
    checksum+=(token+"200");
    checksum+=(token+"1");
    checksum+=(token+info);
    checksum = get_sha1(checksum);
}

