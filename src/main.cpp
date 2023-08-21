#include <iostream>
#include "../include/AuthWorker.h"
#include <ctime>
#include "../include/SrunBase64.h"

int main(int argc, char **argv) {
    AuthWorker au("http://auth4.tsinghua.edu.cn", "135");
    au.auth("aux", "aux");
    return 0;
}
