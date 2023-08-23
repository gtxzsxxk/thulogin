#include <iostream>
#include "../include/Authenticator.h"
#include "../include/PwdInput.h"

/*
 * thulogin, a cross-platform CLI portal authenticating tool
 * Author: hanyuan <2524395907@qq.com>
 *
 * Usages
 * ./thulogin
 * ./thulogin username
 * ./thulogin --wifi --ethernet
 * ./thulogin --username username [--pwd||--password] pwd
 * ./thulogin --server url --ua user-agent --ac-id ac_id
 *
 * ac_id:
 *  ethernet: 173
 *  tsinghua-5g tsinghua: 135
 * */
int argc_value;
int username_acquired = 1;
int password_acquired = 1;
std::string ac_id = "135";
std::string server = "http://auth4.tsinghua.edu.cn";
std::string user, pwd;
std::string user_agent = "Mozilla/5.0 (Windows NT 10.0; Win64; x64)";

int load_parameters(int &ptr, char **argv);

int main(int argc, char **argv) {
    argc_value = argc;
    if (argc > 1) {
        int ptr = 1;
        while (ptr < argc - 1) {
            if (load_parameters(ptr, argv)) {
                std::cerr << "Aborted in parameters parse." << std::endl;
                return -1;
            }
        }
    }
    if (username_acquired) {
        std::cout << "Username:";
        std::cin >> user;
    }
    if (password_acquired) {
        pwd = pwd_input();
    }
    Authenticator au(server, ac_id, user_agent);
    au.auth(user, pwd);
    return 0;
}

int argv_param_validate(int &ptr, char **argv) {
    if (ptr + 1 >= argc_value) {
        return -1;
    }
    if (strlen(argv[ptr + 1]) > 2) {
        if (argv[ptr + 1][0] == '-' && argv[ptr + 1][1] == '-') {
            return -1;
        }
    }
    return 0;
}

int load_parameters(int &ptr, char **argv) {
    if (!strcmp(argv[ptr], "--wifi")) {
        ac_id = "135";
    } else if (!strcmp(argv[ptr], "--ethernet")) {
        ac_id = "173";
    } else if (!strcmp(argv[ptr], "--username")) {
        if (!argv_param_validate(ptr, argv)) {
            user = argv[ptr + 1];
            username_acquired = 0;
            ptr += 2;
            return 0;
        } else {
            std::cerr << "Invalid input near '--username'" << std::endl;
            return -1;
        }
    } else if (!strcmp(argv[ptr], "--pwd") || !strcmp(argv[ptr], "--password")) {
        if (!argv_param_validate(ptr, argv)) {
            pwd = argv[ptr + 1];
            password_acquired = 0;
            ptr += 2;
            return 0;
        } else {
            std::cerr << "Invalid input near '--pwd' or '--password'" << std::endl;
            return -1;
        }
    } else if (!strcmp(argv[ptr], "--server")) {
        if (!argv_param_validate(ptr, argv)) {
            /* TODO: validate the url */
            server = argv[ptr + 1];
            ptr += 2;
            return 0;
        } else {
            std::cerr << "Invalid input near '--server'" << std::endl;
            return -1;
        }
    } else if (!strcmp(argv[ptr], "--ua")) {
        if (!argv_param_validate(ptr, argv)) {
            user_agent = argv[ptr + 1];
            ptr += 2;
            return 0;
        } else {
            std::cerr << "Invalid input near '--ua'" << std::endl;
            return -1;
        }
    } else if (!strcmp(argv[ptr], "--ac-id")) {
        if (!argv_param_validate(ptr, argv)) {
            ac_id = argv[ptr + 1];
            ptr += 2;
            return 0;
        } else {
            std::cerr << "Invalid input near '--ac-id'" << std::endl;
            return -1;
        }
    } else {
        std::cerr << "Unknown parameter: " << argv[ptr] << std::endl;
        return -1;
    }
    ptr++;
    return 0;
}