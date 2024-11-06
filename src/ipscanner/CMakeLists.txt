cmake_minimum_required(VERSION 3.29)
project(ipscanner)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
set(CMAKE_EXE_LINKER_FLAGS "-static")
set(BUILD_SHARED_LIBS OFF)

#include_directories(lib/asio/include/)

add_executable(ipscanner main.cpp)

# Add this to link ws2_32.lib to static library
if (MINGW)
    target_link_libraries(ipscanner iphlpapi ws2_32)
endif ()
#target_link_libraries(server)