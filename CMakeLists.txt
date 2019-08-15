cmake_minimum_required(VERSION 3.8)
project(bftp)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES
        ext/socket.c
        ext/socket.h
        hdr/error.h
        src/util.h
        src/util.c
        src/bftp.c
        src/connection_handler.c
        src/connection_handler.h
        src/control_handler.c
        src/control_handler.h
        src/data_handler.c
        src/data_handler.h)

add_executable(bftp ${SOURCE_FILES})