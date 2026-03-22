cmake_minimum_required(VERSION 3.28)
project(chat)

set(CMAKE_CXX_STANDARD 17)

find_package(Boost REQUIRED)
find_package(wxWidgets CONFIG REQUIRED)
include_directories(${Boost_INCLUDE_DIR})
include_directories(${wxWidgets_INCLUDE_DIR})


add_executable(chat
        main.cpp
        Message.h
        Connection.cpp
        Connection.h
        MsgQ.cpp
        MsgQ.h
        Message.cpp
        main.h
        UIElements/MessageBubble.cpp
        UIElements/MessageBubble.h
)

target_link_libraries(chat PRIVATE wx::core wx::base)