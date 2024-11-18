cmake_minimum_required(VERSION 3.29)
project(RemoteControlViaEmail)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
#set(CMAKE_EXE_LINKER_FLAGS "-static")
set(BUILD_SHARED_LIBS OFF)

#add_subdirectory(lib/wxWidgets-3.2.6)

include_directories(${CMAKE_SOURCE_DIR}/lib/curl/include ${CMAKE_SOURCE_DIR}/lib/openssl/include)
link_directories(${CMAKE_SOURCE_DIR}/lib/curl/lib ${CMAKE_SOURCE_DIR}/lib/openssl/lib)

add_executable(
        RemoteControlViaEmail src/main.cpp
        src/utils/EmailRetrieval.cpp
        src/utils/IniParser.cpp
        src/utils/Client.cpp
        src/utils/GetWinDirectory.cpp
        src/utils/IpDiscovery.cpp
        src/utils/Server.cpp
        src/utils/WebcamController.cpp
        src/utils/Service.cpp
        src/utils/Process.cpp
        src/utils/WindowsCommands.cpp

        #        src/utils/EmailRetrieval.h
        #        src/utils/IniParser.h
        #        src/utils/Client.h
        #        src/utils/GetWinDirectory.h
        #        src/utils/IpDiscovery.h
        #        src/utils/Server.h
        #        src/utils/WebcamController.h
        #        src/utils/Service.h
        #        src/utils/Process.h
        #        src/utils/WindowsCommands.h
        #        src/utils/
)

add_custom_command(
        TARGET RemoteControlViaEmail POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${CMAKE_SOURCE_DIR}/lib/curl/bin/libcurl-x64.dll"
        $<TARGET_FILE_DIR:RemoteControlViaEmail>
)

target_link_libraries(
        RemoteControlViaEmail
        
        curl ssl
        crypto wldap32 advapi32 crypt32 ws2_32 strmiids wsock32 amstrmid
        gdiplus
)