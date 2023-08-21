#pragma once

#include <string>
#include "../include/Types.h"

class Encoder {
public:
    Encoder() {}

    ~Encoder() {}

    std::string UrlEncode(const std::string &str);

    std::string UrlDecode(const std::string &str);

private:
    char CharToInt(char ch);

    char StrToBin(char *pString);
};
