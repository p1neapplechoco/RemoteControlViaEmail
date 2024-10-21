cmake_minimum_required(VERSION 3.29)
project(RemoteControlViaEmail)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
# set(CMAKE_EXE_LINKER_FLAGS "-static")
set(BUILD_SHARED_LIBS OFF)

add_subdirectory(lib/wxWidgets-3.2.6)
include_directories(${CMAKE_SOURCE_DIR}/lib/curl/include ${CMAKE_SOURCE_DIR}/lib/openssl/include)
link_directories(${CMAKE_SOURCE_DIR}/lib/curl/lib ${CMAKE_SOURCE_DIR}/lib/openssl/lib)

add_executable(RemoteControlViaEmail
        src/LoginFrame.cpp)

add_custom_command(TARGET RemoteControlViaEmail POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${CMAKE_SOURCE_DIR}/lib/curl/bin/libcurl-x64.dll"
        $<TARGET_FILE_DIR:RemoteControlViaEmail>)

target_link_libraries(RemoteControlViaEmail wx::net wx::core wx::base curl ssl crypto ws2_32 wldap32 advapi32 crypt32)
