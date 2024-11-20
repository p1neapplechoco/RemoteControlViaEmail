cmake_minimum_required(VERSION 3.29)
project(client)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
set(CMAKE_EXE_LINKER_FLAGS "-static")
set(BUILD_SHARED_LIBS OFF)

#add_executable(client main.cpp
#        cmake-build-debug/Sort.cpp
#        cmake-build-debug/Sort.h)

add_executable(client
        main.cpp
        networkDiscovery.cpp
        ../utils/GetWinDirectory.cpp
)

target_link_libraries(client ws2_32)
#target_link_libraries(server)