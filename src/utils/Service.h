#pragma once

#include <dshow.h>
#include <windows.h>
#include <qedit.h>
#pragma comment(lib, "strmiids")
#pragma comment(lib, "quartz")

#include <dirent.h>
#include <iostream>
#include <propidl.h>
#include <string>
#include <vector>


#ifndef SERVICE_H
#define SERVICE_H

extern ULONG EncoderQuality;
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
