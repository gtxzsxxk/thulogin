//
// Created by hanyuan on 2023/8/22.
//

#include "../include/PwdInput.h"
#include <iostream>

std::string pwd_input(std::string prompt) {
    std::string pwd;
#if defined(_WIN32) || defined(__CYGWIN__)
    std::cout << prompt;
    /* TODO: more elegant way */
    std::cin >> pwd;
#else
    std::cout << prompt;
    struct termios oldt, newt;
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~ECHO;
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    std::getline(std::cin, pwd);
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    std::cout << std::endl;
#endif
    return pwd;
}