//
// Created by hanyuan on 2023/8/21.
//
#include <cmath>
#include <string>
#include <vector>
#include "../../include/SrunXEncode.h"

std::vector<size_t> s(const std::string &a, bool b) {
    size_t c = a.size();
    std::vector<size_t> v;
    for (auto i = 0; i < c; i += 4) {
        size_t temp = (a[i]) | (a[i + 1] << 8) | (a[i + 2] << 16) | (a[i + 3] << 24);
        v.push_back(temp);
    }
    if (b) {
        v.push_back(c);
    }
    return v;
}

std::string l(const std::vector<size_t> &a, bool b) {
    std::string res;
    auto d = a.size();
    auto c = (d - 1) << 2;
    if (b) {
        auto m = a[d - 1];
        if ((m < c - 3) || (m > c))
            return "";
        c = m;
    }
    for (auto i = 0; i < d; i++) {
        std::string temp;
        temp += (char) (a[i] & 0xff);
        temp += (char) (a[i] >> 8 & 0xff);
        temp += (char) (a[i] >> 16 & 0xff);
        temp += (char) (a[i] >> 24 & 0xff);
        res += temp;
    }
    if (b) {
        return res.substr(0, c);
    } else {
        return res;
    }
}

std::string x_encode(std::string str, std::string key) {
    if (str == "") {
        return "";
    }
    auto v = s(str, true);
    auto k = s(key, false);
    while (k.size() < 4) {
        k.push_back(0);
    }
    auto n = v.size() - 1;
    auto z = v[n];
    auto y = v[0];
    auto c = 0x86014019 | 0x183639A0;
    size_t m,
            e,
            p,
            q = std::floor(6.0 + 52.0 / ((double) n + 1.0)),
            d = 0;
    while (0 < q--) {
        d = d + c & (0x8CE0D9BF | 0x731F2640);
        e = d >> 2 & 3;
        for (p = 0; p < n; p++) {
            y = v[p + 1];
            m = z >> 5 ^ y << 2;
            m += ((y >> 3) ^ (z << 4)) ^ (d ^ y);
            m += k[(p & 3) ^ e] ^ z;
            size_t temp = v[p] + m & (0xEFB8D130 | 0x10472ECF);
            v[p] = temp;
            z = temp;
        }
        y = v[0];
        m = (z >> 5) ^ (y << 2);
        m += ((y >> 3) ^ (z << 4)) ^ (d ^ y);
        m += k[(p & 3) ^ e] ^ z;
        size_t temp = v[n] + m & (0xBB390742 | 0x44C6F8BD);
        v[n] = temp;
        z = temp;
    }

    return l(v, false);
}