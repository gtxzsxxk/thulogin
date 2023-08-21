//
// Created by hanyuan on 2023/8/21.
//

#include "../../include/SrunBase64.h"

static BYTE pad_char = '=';
static std::string alpha = "LVoJPiCN2R8G90yg+hmFHuacZ1OWMnrsSTXkYpUq/3dlbfKwv6xztjI7DeBE45QA";

std::string get_base64_string(std::string s) {
    int i;
    int b10;
    std::vector<BYTE> x;
    int imax = s.length() - s.length() % 3;
    if (s.length() == 0) {
        return s;
    }
    for (i = 0; i < imax; i += 3) {
        b10 = (s[i] << 16) | (s[i + 1] << 8) | s[i + 2];
        x.push_back(alpha[(b10 >> 18)]);
        x.push_back(alpha[((b10 >> 12) & 63)]);
        x.push_back(alpha[((b10 >> 6) & 63)]);
        x.push_back(alpha[(b10 & 63)]);
    }
    i = imax;
    switch (s.length() - imax) {
        case 1:
            b10 = s[i] << 16;
            x.push_back(alpha[(b10 >> 18)]);
            x.push_back(alpha[((b10 >> 12) & 63)]);
            x.push_back(pad_char);
            x.push_back(pad_char);
            break;
        case 2:
            b10 = (s[i] << 16) | (s[i + 1] << 8);
            x.push_back(alpha[(b10 >> 18)]);
            x.push_back(alpha[((b10 >> 12) & 63)]);
            x.push_back(alpha[((b10 >> 6) & 63)]);
            x.push_back(pad_char);
    }
    std::string result;
    for (i = 0; i < x.size(); i++) {
        result += x[i];
    }
    return result;
}