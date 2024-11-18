#pragma once

#include <vector>
#include <windows.h>
#include <dirent.h>
#include <iostream>

#ifndef SERVICE_H
#define SERVICE_H

struct ServiceInfo
{
    std::wstring name;
    std::wstring displayName;
    DWORD currentState;
};

class Service
{
public:
    static bool endService(const char *serviceName);

    static std::vector<ServiceInfo> listServices();
};

#endif //SERVICE_H
