cmake_minimum_required(VERSION 3.26)
project(XIVRP-Formatter LANGUAGES CXX)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++20")
set(CMAKE_CXX_STANDARD 20)

# Use pkg-config to get the flags for libwebp
find_package(PkgConfig REQUIRED)
find_package(ZLIB REQUIRED)
find_package(PNG REQUIRED)
pkg_check_modules(WEBP REQUIRED libwebp)

target_include_directories(XIVRP-Formatter PRIVATE ${WEBP_INCLUDE_DIRS})
target_link_libraries(XIVRP-Formatter PNG::PNG ${WEBP_LIBRARIES})

add_executable(XIVRP-Formatter main.cpp
        settings/settings.cpp
        settings/settings.h

        messages/loading.cpp
        messages/loading.h
        messages/messages.cpp
        messages/messages.h
        messages/message.cpp
        messages/message.h

        images/related_images.cpp
        images/related_images.h

        templating/templating.cpp
        templating/templating.h

        includes/json.hpp
        includes/date.h
        includes/NLTemplate.cpp
        includes/NLTemplate.h
        includes/png-base64-webp.h
)
