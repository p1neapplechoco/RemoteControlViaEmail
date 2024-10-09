//
// Created by phida on 10/9/2024.
//


#include "server.h"

using namespace std;

// for display purpose
std::wstring getStateString(DWORD state) {
    switch (state) {
        case SERVICE_STOPPED: return L"Stopped";
        case SERVICE_START_PENDING: return L"Start Pending";
        case SERVICE_STOP_PENDING: return L"Stop Pending";
        case SERVICE_RUNNING: return L"Running";
        case SERVICE_CONTINUE_PENDING: return L"Continue Pending";
        case SERVICE_PAUSE_PENDING: return L"Pause Pending";
        case SERVICE_PAUSED: return L"Paused";
        default: return L"Unknown";
    }
}

void Server::handleClient(SOCKET clientSocket) {
    char buffer[1024];
    while (true) {
        memset(buffer, 0, sizeof(buffer));
        int bytesReceived = recv(clientSocket, buffer, sizeof(buffer), 0);
        if (bytesReceived <= 0) {
            cout << "Client disconnected." << endl;
            break;
        }

        cout << "Received: " << buffer << endl;

        // [Feature number 1] List all applications
        if (strcmp(buffer, "list app") == 0) {
            std::vector<ProcessInfo> processes = listApplications();
            std::map<ProcessType, std::vector<ProcessInfo> > groupedProcesses;

            for (const auto &process: processes) {
                groupedProcesses[process.type].push_back(process);
            }

            const wchar_t *typeNames[] = {L"Apps", L"Background processes", L"Windows processes"};

            for (int i = 0; i < 3; ++i) {
                ProcessType type = static_cast<ProcessType>(i);
                std::wcout << typeNames[i] << L" (" << groupedProcesses[type].size() << L")" << std::endl;
                std::wcout << std::wstring(50, L'-') << std::endl;

                for (const auto &process: groupedProcesses[type]) {
                    std::wcout << std::left << std::setw(10) << process.pid << process.name << std::endl;
                }
                std::wcout << std::endl;
            }

            std::wcout << L"Total processes: " << processes.size() << std::endl;
        } else if (strcmp(buffer, "list service") == 0) {
            std::vector<ServiceInfo> services = listServices();

            std::wcout << std::left << std::setw(40) << L"Service Name"
                    << std::setw(50) << L"Display Name"
                    << L"State" << std::endl;
            std::wcout << std::wstring(100, L'-') << std::endl;

            for (const auto &service: services) {
                std::wcout << std::left << std::setw(40) << service.name
                        << std::setw(50) << service.displayName
                        << getStateString(service.currentState) << std::endl;
            }

            std::wcout << L"\nTotal services: " << services.size() << std::endl;
        } else if (strcmp(buffer, "screen capture") == 0) {
            screenCapture();
        } else if (strcmp(buffer, "shutdown") == 0) {
            shutdown();
        } else if (strcmp(buffer, "view file") == 0) {
            viewFile();
        } else if (strcmp(buffer, "get file") == 0) {
            getFile();
        } else {
        }

        if (strcmp(buffer, "exit") == 0) {
            cout << "Client requested to exit." << endl;
            break;
        }

        string response = string(buffer);
        transform(response.begin(), response.end(), response.begin(), ::toupper);
        send(clientSocket, response.c_str(), response.length(), 0);
    }
    closesocket(clientSocket);
}

Server::Server() {
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "Failed to initialize Winsock" << std::endl;
        return;
    }

    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == INVALID_SOCKET) {
        std::cerr << "Failed to create socket" << std::endl;
        WSACleanup();
        return;
    }

    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = 0;

    if (bind(serverSocket, (sockaddr *) &serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        std::cerr << "Bind failed" << std::endl;
        closesocket(serverSocket);
        WSACleanup();
        return;
    }

    // Get the assigned port
    int addrLen = sizeof(serverAddr);
    if (getsockname(serverSocket, (sockaddr *) &serverAddr, &addrLen) == SOCKET_ERROR) {
        cerr << "Failed to get socket name" << endl;
        closesocket(serverSocket);
        WSACleanup();
        return;
    }
    assignedPort = ntohs(serverAddr.sin_port);
}

Server::~Server() {
    cout << "Server destroyed" << endl;
}

void Server::startListening() {
    if (listen(serverSocket, SOMAXCONN) == SOCKET_ERROR) {
        std::cerr << "Listen failed" << std::endl;
        closesocket(serverSocket);
        WSACleanup();
        return;
    }
    cout << "Server listening on port " << assignedPort << std::endl;

    // Receive and send messages
    while (true) {
        SOCKET clientSocket = accept(serverSocket, NULL, NULL);
        if (clientSocket == INVALID_SOCKET) {
            cerr << "Accept failed" << endl;
            continue;
        }
        cout << "New client connected." << endl;
        handleClient(clientSocket);
    }
}

std::vector<ProcessInfo> Server::listApplications() {
    std::vector<ProcessInfo> processes;
    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hSnapshot == INVALID_HANDLE_VALUE) {
        throw std::runtime_error("CreateToolhelp32Snapshot failed");
    }

    PROCESSENTRY32W pe32;
    pe32.dwSize = sizeof(PROCESSENTRY32W);

    if (!Process32FirstW(hSnapshot, &pe32)) {
        CloseHandle(hSnapshot);
        throw std::runtime_error("Process32First failed");
    }

    do {
        HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, pe32.th32ProcessID);
        if (hProcess) {
            wchar_t szProcessPath[MAX_PATH];
            if (GetModuleFileNameExW(hProcess, NULL, szProcessPath, MAX_PATH)) {
                ProcessType type;
                if (wcsstr(szProcessPath, L"\\Windows\\") != nullptr) {
                    type = ProcessType::WindowsProcess;
                } else if (pe32.cntThreads > 1) {
                    type = ProcessType::App;
                } else {
                    type = ProcessType::BackgroundProcess;
                }
                processes.push_back({pe32.th32ProcessID, pe32.szExeFile, type});
            }
            CloseHandle(hProcess);
        }
    } while (Process32NextW(hSnapshot, &pe32));

    CloseHandle(hSnapshot);
    return processes;
}


std::vector<ServiceInfo> Server::listServices() {
    std::vector<ServiceInfo> services;

    SC_HANDLE hSCManager = OpenSCManagerW(nullptr, nullptr, SC_MANAGER_ENUMERATE_SERVICE);
    if (hSCManager == nullptr) {
        throw std::runtime_error("Failed to open Service Control Manager");
    }

    DWORD bytesNeeded = 0;
    DWORD servicesReturned = 0;
    DWORD resumeHandle = 0;
    ENUM_SERVICE_STATUS_PROCESSW *pServices = nullptr;

    // First call to get required buffer size
    EnumServicesStatusExW(hSCManager, SC_ENUM_PROCESS_INFO, SERVICE_WIN32, SERVICE_STATE_ALL,
                          nullptr, 0, &bytesNeeded, &servicesReturned, &resumeHandle, nullptr);

    pServices = (ENUM_SERVICE_STATUS_PROCESSW *) new char[bytesNeeded];

    if (!EnumServicesStatusExW(hSCManager, SC_ENUM_PROCESS_INFO, SERVICE_WIN32, SERVICE_STATE_ALL,
                               (LPBYTE) pServices, bytesNeeded, &bytesNeeded, &servicesReturned,
                               &resumeHandle, nullptr)) {
        delete[] pServices;
        CloseServiceHandle(hSCManager);
        throw std::runtime_error("Failed to enumerate services");
    }

    for (DWORD i = 0; i < servicesReturned; i++) {
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

void Server::screenCapture() {
    return;
};

void Server::shutdown() {
    return;
};

void Server::viewFile() {
    return;
};

void Server::getFile() {
    return;
};
