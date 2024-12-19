
cmake_minimum_required(VERSION 3.29)
project(ipscanner)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
set(CMAKE_EXE_LINKER_FLAGS "-static")
set(BUILD_SHARED_LIBS OFF)

#add_subdirectory(lib/wxWidgets-3.2.6)
#
#include_directories(${CMAKE_SOURCE_DIR}/lib/curl/include ${CMAKE_SOURCE_DIR}/lib/openssl/include)
#link_directories(${CMAKE_SOURCE_DIR}/lib/curl/lib ${CMAKE_SOURCE_DIR}/lib/openssl/lib)

add_executable(ipscanner main.cpp)

target_link_libraries(ipscanner ws2_32 wldap32 advapi32 crypt32)