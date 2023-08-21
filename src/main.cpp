#include <iostream>
#include <cstring>
#include "../include/AuthWorker.h"

/*
 * thulogin, a cross-platform CLI portal authenticating tool
 * Author: hanyuan <2524395907@qq.com>
 *
 * Usages
 * ./thulogin
 * ./thulogin username password
 * ./thulogin --specify-baseurl baseurl --specify-ua user-agent --specify-ac-id ac_id
 * */
int main(int argc, char **argv) {
    AuthWorker * au;
    if(argc==1){

    }
    au->auth("aux", "aux");
    delete au;
    return 0;
}
