cmake_minimum_required(VERSION 3.28)
project(chat)

set(CMAKE_CXX_STANDARD 17)

find_package(Boost 1.84 REQUIRED)
include_directories(${Boost_INCLUDE_DIR})

add_executable(chat main.cpp
        Message.h
        Connection.cpp
        Connection.h
        MsgQ.cpp
        MsgQ.h
        Message.cpp)
