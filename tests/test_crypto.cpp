#include <iostream>
#include <cassert>
#include <string>

#include "../include/SrunMd5.h"
#include "../include/SrunSHA1.h"
#include "../include/SrunBase64.h"
#include "../include/SrunXEncode.h"

void test_md5() {
    // MD5("test") = "098f6bcd4621d373cade4e832627b4f6"
    std::string result = get_md5("test");
    assert(result == "098f6bcd4621d373cade4e832627b4f6");
    std::cout << "[PASS] MD5 test" << std::endl;
}

void test_sha1() {
    // SHA1("test") = "a94a8fe5ccb19ba61c4c0873d391e987982fbbd3"
    std::string result = get_sha1("test");
    assert(result == "a94a8fe5ccb19ba61c4c0873d391e987982fbbd3");
    std::cout << "[PASS] SHA1 test" << std::endl;
}

void test_base64() {
    // Test QuirkBase64 encoding
    std::string input = "hello";
    std::string result = get_base64_string(input);
    // Result should be non-empty and different from standard base64
    assert(!result.empty());
    std::cout << "[PASS] Base64 test (result: " << result << ")" << std::endl;
}

void test_xencode() {
    // Test x_encode with simple input
    std::string encoded = x_encode("test", "token123");
    assert(!encoded.empty());
    std::cout << "[PASS] XEncode test" << std::endl;
}

int main() {
    std::cout << "Running crypto unit tests..." << std::endl;
    test_md5();
    test_sha1();
    test_base64();
    test_xencode();
    std::cout << "All tests passed!" << std::endl;
    return 0;
}
