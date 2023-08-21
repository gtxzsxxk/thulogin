//
// Created by hanyuan on 2023/8/21.
//

#include "../../include/SrunMd5.h"

std::string get_hmac_md5(const std::string & password, const std::string & token){
    return hmac<MD5>(password,token);
}