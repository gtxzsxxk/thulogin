#include <iostream>
#include "../include/SrunBase64.h"
#include "../include/SrunMd5.h"
#include "../include/SrunSHA1.h"

int main() {
    std::cout << get_sha1("18915649841") << std::endl;
    return 0;
}
