cmake_minimum_required(VERSION 3.26)
project(ff_rp_formatter LANGUAGES CXX)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++20")
set(CMAKE_CXX_STANDARD 20)

add_executable(ff_rp_formatter main.cpp
        includes/settings.cpp
        includes/settings.h
        includes/messages/messages.cpp
        includes/messages/messages.h
        includes/messages/message.cpp
        includes/messages/message.h
        includes/messages/loading.cpp
        includes/messages/loading.h
        includes/json.hpp
        includes/date.h
        includes/templating.cpp
        includes/templating.h
)
