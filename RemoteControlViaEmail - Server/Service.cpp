#include "Service.h"

bool Service::startService(const char *serviceName)
{
    const auto hSCManager = OpenSCManager(nullptr, nullptr, SC_MANAGER_CONNECT);
    if (hSCManager == nullptr)
    {
        std::cerr << "Failed to open service control manager. Error: " << GetLastError() << std::endl;
        return false;
    }

    const int length = MultiByteToWideChar(CP_UTF8, 0, serviceName, -1, NULL, 0);
    std::wstring wServiceName(length, 0);
    MultiByteToWideChar(CP_UTF8, 0, serviceName, -1, &wServiceName[0], length);

    const auto hService = OpenServiceW(hSCManager, wServiceName.c_str(), SERVICE_START | SERVICE_QUERY_STATUS);
    if (hService == nullptr)
    {
        std::cerr << "Failed to open service: " << serviceName << ". Error: " << GetLastError() << std::endl;
        CloseServiceHandle(hSCManager);
        return false;
    }

    if (StartService(hService, 0, nullptr))
    {
        std::cout << "Service " << serviceName << " starting...\n";

        // Wait until the service is started
        SERVICE_STATUS status;
        while (QueryServiceStatus(hService, &status))
        {
            if (status.dwCurrentState == SERVICE_RUNNING)
            {
                std::cout << "Service " << serviceName << " started successfully." << std::endl;
                break;
            }
            Sleep(1000);
        }
    } else
    {
        std::cerr << "StartService failed. Error: " << GetLastError() << std::endl;
    }

    CloseServiceHandle(hService);
    CloseServiceHandle(hSCManager);
    return true;
}

bool Service::endService(const char *serviceName)
{
    const auto hSCManager = OpenSCManager(nullptr, nullptr, SC_MANAGER_CONNECT);
    if (hSCManager == nullptr)
    {
        std::cerr << "Failed to open service control manager. Error: " << GetLastError() << std::endl;
        return false;
    }

    const auto hService = OpenService(hSCManager, serviceName, SERVICE_STOP | SERVICE_QUERY_STATUS);
    if (hService == nullptr)
    {
        std::cerr << "Failed to open service: " << serviceName << ". Error: " << GetLastError() << std::endl;
        CloseServiceHandle(hSCManager);
        return false;
    }

    SERVICE_STATUS status;
    if (ControlService(hService, SERVICE_CONTROL_STOP, &status))
    {
        std::cout << "Service " << serviceName << " stopping...\n";

        // Wait until the service is stopped
        while (QueryServiceStatus(hService, &status))
        {
            if (status.dwCurrentState == SERVICE_STOPPED)
            {
                std::cout << "Service " << serviceName << " stopped successfully." << std::endl;
                break;
            }
            Sleep(1000);
        }
    }
    else
    {
        std::cerr << "ControlService failed. Error: " << GetLastError() << std::endl;
    }

    CloseServiceHandle(hService);
    CloseServiceHandle(hSCManager);
    return true;
}

std::vector<ServiceInfo> Service::listServices()
{
    std::vector<ServiceInfo> services;

    const auto hSCManager = OpenSCManagerW(nullptr, nullptr, SC_MANAGER_ENUMERATE_SERVICE);
    if (hSCManager == nullptr)
    {
        throw std::runtime_error("Failed to open Service Control Manager");
    }

    DWORD bytesNeeded = 0;
    DWORD servicesReturned = 0;
    DWORD resumeHandle = 0;
    ENUM_SERVICE_STATUS_PROCESSW *pServices = nullptr;

    // First call to get required buffer size
    EnumServicesStatusExW(hSCManager, SC_ENUM_PROCESS_INFO, SERVICE_WIN32, SERVICE_STATE_ALL,
                          nullptr, 0, &bytesNeeded, &servicesReturned, &resumeHandle, nullptr);

    pServices = reinterpret_cast<ENUM_SERVICE_STATUS_PROCESSW *>(new char[bytesNeeded]);

    if (!EnumServicesStatusExW(hSCManager, SC_ENUM_PROCESS_INFO, SERVICE_WIN32, SERVICE_STATE_ALL,
                               reinterpret_cast<LPBYTE>(pServices), bytesNeeded, &bytesNeeded, &servicesReturned,
                               &resumeHandle, nullptr))
    {
        delete[] pServices;
        CloseServiceHandle(hSCManager);
        throw std::runtime_error("Failed to enumerate services");
    }

    for (DWORD i = 0; i < servicesReturned; i++)
    {
        ServiceInfo info;
        info.name = pServices[i].lpServiceName;
        info.displayName = pServices[i].lpDisplayName;
        info.currentState = pServices[i].ServiceStatusProcess.dwCurrentState;
        services.push_back(info);
    }

    delete[] pServices;

    CloseServiceHandle(hSCManager);

    return services;
};
