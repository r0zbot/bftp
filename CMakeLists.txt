cmake_minimum_required(VERSION 3.8)
project(bftp)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES
        ext/socket.c
        ext/socket.h
        hdr/error.h
        hdr/util.h
        src/bftp.c
        src/client_handler.c
        src/client_handler.h
        src/connection_handler.c
        src/connection_handler.h)

add_executable(bftp ${SOURCE_FILES})