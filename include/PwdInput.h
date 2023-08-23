//
// Created by hanyuan on 2023/8/22.
//

#ifndef THULOGIN_PWDINPUT_H
#define THULOGIN_PWDINPUT_H

#include <string>

#if defined(_WIN32) || defined(__CYGWIN__)
#include <iostream>
#include <conio.h>
#else

#include <unistd.h>

#endif

std::string pwd_input(std::string prompt = "Password:");

#endif //THULOGIN_PWDINPUT_H
