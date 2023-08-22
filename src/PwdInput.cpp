//
// Created by hanyuan on 2023/8/22.
//

#include "../include/PwdInput.h"

std::string pwd_input(std::string prompt) {
    std::string pwd;
#ifdef MSC_VER
    char ch='\0';
    while(ch!='\n'){
        ch=getch();
        pwd+=ch;
    }
#else
    pwd = getpass(prompt.c_str());
#endif
    return pwd;
}