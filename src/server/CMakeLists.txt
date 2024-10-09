cmake_minimum_required(VERSION 3.29)
project(server)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
set(CMAKE_EXE_LINKER_FLAGS "-static")
set(BUILD_SHARED_LIBS OFF)

add_executable(server main.cpp main.h server.cpp server.h)

# Add this to link ws2_32.lib to static library
if(MINGW)
    target_link_libraries(server ws2_32)
endif()
#target_link_libraries(server)