cmake_minimum_required(VERSION 3.29)
project(ip)

set(CMAKE_CXX_STANDARD 14)

add_executable(ip
    main.cpp)

target_link_libraries(ip ws2_32)