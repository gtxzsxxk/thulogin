#include <iostream>
#include <cstring>
#include "../include/AuthWorker.h"

/*
 * thulogin, a cross-platform CLI portal authenticating tool
 * Author: hanyuan <2524395907@qq.com>
 *
 * Usages
 * ./thulogin
 * ./thulogin username password
 * ./thulogin --specify-server url --specify-ua user-agent --specify-ac-id ac_id
 * */
int main(int argc, char **argv) {
    AuthWorker au("http://auth4.tsinghua.edu.cn", "135");
    std::string user, pwd;
    if (argc == 1) {
        std::cout << "Username:";
        std::cin >> user;

        std::cout << "Password:";
        std::cin >> pwd;
    }
    au.auth(user, pwd);
    return 0;
}
