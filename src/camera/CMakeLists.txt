cmake_minimum_required(VERSION 3.29)
project(camera)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
set(CMAKE_EXE_LINKER_FLAGS "-static")
set(BUILD_SHARED_LIBS OFF)

add_executable(camera main.cpp)

# Add this to link ws2_32.lib to static library
if(MINGW)
    target_link_libraries(camera strmiids ole32 oleaut32)
endif()
#target_link_libraries(server)