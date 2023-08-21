//
// Created by hanyuan on 2023/8/21.
//

#include "../../include/SrunSHA1.h"
#include "../../lib/hash-lib/sha1.h"

std::string get_sha1(const std::string & value){
    return SHA1()(value);
}