#include <iostream>
#include "../include/AuthWorker.h"
#include "../include/PwdInput.h"

/*
 * thulogin, a cross-platform CLI portal authenticating tool
 * Author: hanyuan <2524395907@qq.com>
 *
 * Usages
 * ./thulogin
 * ./thulogin username
 * ./thulogin --username username --pwd pwd
 * ./thulogin --specify-server url --specify-ua user-agent --specify-ac-id ac_id
 * */
int main(int argc, char **argv) {
    AuthWorker au("http://auth4.tsinghua.edu.cn", "135");
    std::string user, pwd;
    if (argc == 1) {
        std::cout << "Username:";
        std::cin >> user;

        pwd = pwd_input();
    } else if (argc == 3) {

    }
    au.auth(user, pwd);
    return 0;
}
