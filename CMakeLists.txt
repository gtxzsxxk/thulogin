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
        lib/HTTPRequest.hpp
        src/encrypt/SrunSHA1.cpp
        include/SrunSHA1.h
        src/Authenticator.cpp
        include/Authenticator.h
        src/encrypt/SrunXEncode.cpp
        include/SrunXEncode.h
        lib/Encoder.h
        lib/Encoder.cpp
        src/PwdInput.cpp
        include/PwdInput.h
)

IF (WIN32)
        target_link_libraries(thulogin wsock32 ws2_32)
ENDIF ()