//
// Created by hanyuan on 2023/8/21.
//

#ifndef THULOGIN_SRUNMD5_H
#define THULOGIN_SRUNMD5_H

#include "../lib/hash-lib/hmac.h"
#include "../lib/hash-lib/md5.h"

std::string get_hmac_md5(const std::string &password, const std::string &token);

#endif //THULOGIN_SRUNMD5_H
