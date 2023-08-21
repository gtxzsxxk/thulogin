#include <iostream>
#include "../include/AuthWorker.h"
#include <ctime>
#include "../include/SrunBase64.h"

int main() {
    AuthWorker au("http://auth4.tsinghua.edu.cn","135");
    au.auth("aux","test");
    return 0;
}
