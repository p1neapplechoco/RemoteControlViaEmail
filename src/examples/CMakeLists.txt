cmake_minimum_required(VERSION 3.29)
project(examples)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
set(CMAKE_EXE_LINKER_FLAGS "-static")
set(BUILD_SHARED_LIBS OFF)

add_executable(examples
        folder_traversal.cpp
        ../utils/GetWinDirectory.cpp
)

target_link_libraries(examples ws2_32)
