cmake_minimum_required(VERSION 3.29)
project(RemoteControlViaEmail)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
set(BUILD_SHARED_LIBS OFF)

add_subdirectory(lib/wxWidgets-3.2.6)
add_executable(RemoteControlViaEmail
        src/App.cpp
        src/App.h
        src/MainFrame.cpp
        src/MainFrame.h)

target_link_libraries(RemoteControlViaEmail wx::net wx::core wx::base)
