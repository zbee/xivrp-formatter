cmake_minimum_required(VERSION 3.26)
project(XIVRP-Formatter LANGUAGES CXX)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++20")
set(CMAKE_CXX_STANDARD 20)

add_executable(XIVRP-Formatter main.cpp
        settings/settings.cpp
        settings/settings.h
        messages/messages.cpp
        messages/messages.h
        messages/message.cpp
        messages/message.h
        messages/loading.cpp
        messages/loading.h
        includes/json.hpp
        includes/date.h
        templating/templating.cpp
        templating/templating.h
)
