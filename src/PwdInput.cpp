//
// Created by hanyuan on 2023/8/22.
//

#include "../include/PwdInput.h"

std::string pwd_input(std::string prompt) {
    std::string pwd;
#ifdef __WIN32__
    std::cout << prompt;
    /* TODO: more elegant way */
    std::cin >> pwd;
#else
    pwd = getpass(prompt.c_str());
#endif
    return pwd;
}