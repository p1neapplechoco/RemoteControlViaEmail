#pragma once

#include <iomanip>
#include <string>
#include <vector>
#include <winsock2.h>
#include <windows.h>
#include <dirent.h>
#include <functional>
#include <io.h>
#include <iostream>
#include <psapi.h>
#include <thread>
#include <tlhelp32.h>
#include <unistd.h>
#include <winsvc.h>
#include <ws2tcpip.h>
#include <sys/types.h>

#ifndef PROCESS_H
#define PROCESS_H

enum class ProcessType
{
    App,
    BackgroundProcess,
    WindowsProcess
};

struct ProcessInfo
{
    DWORD pid;
    std::wstring name;
    ProcessType type;
};

std::wstring getStateString(DWORD state);

class Process
{
public:
    static bool endProcess(int process_id);

    static std::vector<ProcessInfo> listProcesses();
};


#endif //PROCESS_H
