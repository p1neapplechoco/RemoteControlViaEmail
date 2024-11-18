cmake_minimum_required(VERSION 3.29)
project(server)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
#set(CMAKE_EXE_LINKER_FLAGS "-static")
set(BUILD_SHARED_LIBS OFF)

#include_directories(lib/asio/include/)

add_executable(server main.cpp main.h server.cpp server.h discoveryResponder.cpp discoveryResponder.h
        WebcamController.cpp
        WebcamController.h
        Service.cpp
        Service.h
        Process.cpp
        Process.h
        WindowsCommands.cpp
        WindowsCommands.h)

# Add this to link ws2_32.lib to static library
if (MINGW)
    target_link_libraries(server ws2_32 strmiids gdiplus wsock32 amstrmid)
endif ()
#target_link_libraries(server)