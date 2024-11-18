//
// Created by phine on 17/11/2024.
//
#pragma once

#ifndef WINDOWSCOMMANDS_H

#include <winsock2.h>
#include <windows.h>
#include <winsvc.h>
#include <psapi.h>
#include <tlhelp32.h>
#include <ws2tcpip.h>
#include <vector>
#include <string>
#include <map>
#include <algorithm>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <gdiplus.h>
#include <windows.h>
#include <fcntl.h>
#include <io.h>
// #include <asio.hpp>
#include <functional>
#include <thread>
#include <chrono>
#include <assert.h>

#define WINDOWSCOMMANDS_H

class WindowsCommands
{
public:
    static std::vector<char> screenShot();

    static void shutdown(UINT nSDType = 0);
};


#endif //WINDOWSCOMMANDS_H
