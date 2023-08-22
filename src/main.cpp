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
 *
 * ac_id:
 *  ethernet: 173
 *  tsinghua-5g tsinghua: 135
 * */
int main(int argc, char **argv) {
    AuthWorker au("http://auth4.tsinghua.edu.cn", "135");
    std::string user, pwd;
    if (argc == 1) {
        std::cout << "Username:";
        std::cin >> user;

        pwd = pwd_input();
    } else if (argc == 2) {
        user=std::string(argv[1]);
        pwd = pwd_input();
    }

    au.auth(user, pwd);
    return 0;
}
