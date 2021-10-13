cmake_minimum_required(VERSION 2.9)

project("libreONLINE")
include_directories(raylib/include
                    src
                    )

set(CMAKE_BUILD_TYPE debug)


add_executable(libreonline
                    src/client.cpp
                    )

target_link_libraries(libreonline
                    ${CMAKE_SOURCE_DIR}/raylib/lib/libraylib.a
                    dl
                    pthread
                    )


