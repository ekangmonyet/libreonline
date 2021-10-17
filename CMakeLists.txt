cmake_minimum_required(VERSION 2.9)

project("libreONLINE")
include_directories(raylib/include
                    nbnet/include
                    src
                    )

set(CMAKE_BUILD_TYPE debug)

pkg_check_modules(RAYLIB REQUIRED raylib)


add_executable(libreonline
                    src/client.cpp
                    )

target_link_libraries(libreonline
                    ${RAYLIB_LIBRARIES}
                    dl
                    pthread
                    )

