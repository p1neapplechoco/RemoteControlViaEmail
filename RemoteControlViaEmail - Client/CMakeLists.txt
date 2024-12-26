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

add_executable(
        RemoteControlViaEmail src/main.cpp
        src/utils/Email.cpp
        src/utils/IniParser.cpp
        src/utils/Client.cpp
        src/utils/networkDiscovery.cpp
        src/utils/IpDiscovery.cpp
        src/utils/GetWinDirectory.cpp

        src/app/CustomBitmapButton.cpp
        src/app/GmailLoginDialog.cpp
        src/app/LoginFrame.cpp
        src/app/MainFrame.cpp
        src/app/LogPanel.cpp
        src/app/FileExplorer.cpp
        src/app/ProcessManager.cpp
        src/app/ServiceManager.cpp
        src/app/App.h
)

if(EXISTS ${CMAKE_BINARY_DIR}/assert)
    file(REMOVE_RECURSE ${CMAKE_BINARY_DIR}/assert)
endif()
file(COPY ${CMAKE_SOURCE_DIR}/assert DESTINATION ${CMAKE_BINARY_DIR})

add_custom_command(
        TARGET RemoteControlViaEmail POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${CMAKE_SOURCE_DIR}/lib/curl/bin/libcurl-x64.dll"
        $<TARGET_FILE_DIR:RemoteControlViaEmail>
)

target_link_libraries(RemoteControlViaEmail wx::net wx::core wx::base curl ssl crypto
        ws2_32 wldap32 advapi32 crypt32 strmiids quartz ole32 oleaut32 uuid gdi32
        wsock32 amstrmid gdiplus)