cmake_minimum_required(VERSION 3.26)
project(XIVRP-Formatter LANGUAGES CXX)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++20")
set(CMAKE_CXX_STANDARD 20)

add_executable(XIVRP-Formatter main.cpp
        includes/json.hpp
        includes/date.h
        #includes/NLTemplate.cpp
        #includes/NLTemplate.h
        includes/base64.hpp

        common/utilities.cpp
        common/utilities.h

        settings/settings.cpp
        settings/settings.h
        settings/ask.cpp
        settings/ask.h

        messages/loading.cpp
        messages/loading.h
        messages/messages.cpp
        messages/messages.h
        messages/message.cpp
        messages/message.h
        messages/gaps.cpp
        messages/gaps.h

        images/related_images.cpp
        images/related_images.h

        templating/templating.cpp
        templating/templating.h
        # TODO: Switch to NLTemplate
)
