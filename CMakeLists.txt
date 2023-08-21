cmake_minimum_required(VERSION 3.26)
project(thulogin)

set(CMAKE_CXX_STANDARD 17)

add_executable(thulogin src/main.cpp
        src/encrypt/SrunBase64.cpp
        include/SrunBase64.h
        include/Types.h
        src/encrypt/SrunMd5.cpp
        include/SrunMd5.h
        lib/hash-lib/md5.h
        lib/hash-lib/md5.cpp
        lib/hash-lib/sha1.cpp
        lib/hash-lib/sha1.h
        src/encrypt/SrunSHA1.cpp
        include/SrunSHA1.h
        src/AuthInfo.cpp
        include/AuthInfo.h
        lib/HTTPRequest.hpp)
