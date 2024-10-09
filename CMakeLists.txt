cmake_minimum_required(VERSION 3.29)
project(RemoteControlViaEmail)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
set(BUILD_SHARED_LIBS OFF)

add_subdirectory(wxWidgets-3.2.6)
add_executable(RemoteControlViaEmail main.cpp)

target_link_libraries(RemoteControlViaEmail wx::net wx::core wx::base)
#if(WIN32)
#    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /MANIFEST:NO")
#endif()
#add_executable(RemoteControlViaEmail main.cpp)
#
## Include directories
#target_include_directories(RemoteControlViaEmail PUBLIC
#        D:/Repository/CUUTAO/lib/wxT/include
#        D:/Repository/CUUTAO/lib/wxT/lib/gcc810_x64_dll/mswu
#)
#
## Set wxWidgets library directory
#set(WX_LIB_DIR D:/Repository/CUUTAO/lib/wxT/lib/gcc810_x64_dll)
#
## List of wxWidgets libraries to link (without file extensions)
#set(WX_LIBS
#        wxbase32u
#        wxbase32u_net
#        wxbase32u_xml
#        wxmsw32u_adv
#        wxmsw32u_aui
#        wxmsw32u_core
#        wxmsw32u_gl
#        wxmsw32u_html
#        wxmsw32u_media
#        wxmsw32u_propgrid
#        wxmsw32u_ribbon
#        wxmsw32u_richtext
#        wxmsw32u_stc
#        wxmsw32u_webview
#        wxmsw32u_xrc
#)
#
## Additional libraries
#set(ADDITIONAL_LIBS
#        wxexpat
#        wxjpeg
#        wxpng
#        wxregexu
#        wxscintilla
#        wxtiff
#        wxzlib
#)
#
## Find and link wxWidgets libraries
#foreach(lib ${WX_LIBS})
#    find_library(${lib}_PATH
#            NAMES
#            ${lib}_gcc810_x64  # DLL
#            lib${lib}          # Static library
#            PATHS ${WX_LIB_DIR}
#            NO_DEFAULT_PATH
#    )
#    if(${lib}_PATH)
#        target_link_libraries(RemoteControlViaEmail PRIVATE ${${lib}_PATH})
#    else()
#        message(WARNING "Library ${lib} not found")
#    endif()
#endforeach()
#
## Find and link additional libraries
#foreach(lib ${ADDITIONAL_LIBS})
#    find_library(${lib}_PATH
#            NAMES
#            lib${lib}
#            PATHS ${WX_LIB_DIR}
#            NO_DEFAULT_PATH
#    )
#    if(${lib}_PATH)
#        target_link_libraries(RemoteControlViaEmail PRIVATE ${${lib}_PATH})
#    else()
#        message(WARNING "Additional library ${lib} not found")
#    endif()
#endforeach()
#
## Add debug libraries if needed
#if(CMAKE_BUILD_TYPE STREQUAL "Debug")
#    set(WX_DEBUG_LIBS
#            wxbase32ud
#            wxbase32ud_net
#            wxbase32ud_xml
#            wxmsw32ud_adv
#            wxmsw32ud_aui
#            wxmsw32ud_core
#            wxmsw32ud_gl
#            wxmsw32ud_html
#            wxmsw32ud_media
#            wxmsw32ud_propgrid
#            wxmsw32ud_ribbon
#            wxmsw32ud_richtext
#            wxmsw32ud_stc
#            wxmsw32ud_webview
#            wxmsw32ud_xrc
#    )
#
#    foreach(lib ${WX_DEBUG_LIBS})
#        find_library(${lib}_PATH
#                NAMES
#                ${lib}_gcc810_x64  # DLL
#                lib${lib}          # Static library
#                PATHS ${WX_LIB_DIR}
#                NO_DEFAULT_PATH
#        )
#        if(${lib}_PATH)
#            target_link_libraries(RemoteControlViaEmail PRIVATE ${${lib}_PATH})
#        else()
#            message(WARNING "Debug library ${lib} not found")
#        endif()
#    endforeach()
#
#    # Additional debug libraries
#    set(ADDITIONAL_DEBUG_LIBS
#            wxexpatd
#            wxjpegd
#            wxpngd
#            wxregexud
#            wxscintillad
#            wxtiffd
#            wxzlibd
#    )
#
#    foreach(lib ${ADDITIONAL_DEBUG_LIBS})
#        find_library(${lib}_PATH
#                NAMES
#                lib${lib}
#                PATHS ${WX_LIB_DIR}
#                NO_DEFAULT_PATH
#        )
#        if(${lib}_PATH)
#            target_link_libraries(RemoteControlViaEmail PRIVATE ${${lib}_PATH})
#        else()
#            message(WARNING "Additional debug library ${lib} not found")
#        endif()
#    endforeach()
#endif()