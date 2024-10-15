//
// Created by phida on 10/9/2024.
//


#include "server.h"

using namespace std;

// Helper function to get the encoder CLSID for a given image format
int GetEncoderClsid(const WCHAR* format, CLSID* pClsid) {
    UINT num = 0;
    UINT size = 0;

    Gdiplus::ImageCodecInfo* pImageCodecInfo = NULL;

    Gdiplus::GetImageEncodersSize(&num, &size);
    if (size == 0)
        return -1;

    pImageCodecInfo = (Gdiplus::ImageCodecInfo*)(malloc(size));
    if (pImageCodecInfo == NULL)
        return -1;

    Gdiplus::GetImageEncoders(num, size, pImageCodecInfo);

    for (UINT j = 0; j < num; ++j) {
        if (wcscmp(pImageCodecInfo[j].MimeType, format) == 0) {
            *pClsid = pImageCodecInfo[j].Clsid;
            free(pImageCodecInfo);
            return j;
        }
    }

    free(pImageCodecInfo);
    return -1;
}

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

#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>
#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <iomanip>
#include <sstream>
#include <algorithm>

#pragma comment(lib, "ws2_32.lib")

void Server::handleClient(SOCKET clientSocket) {
    char buffer[1024];
    while (true) {
        memset(buffer, 0, sizeof(buffer));
        int bytesReceived = recv(clientSocket, buffer, sizeof(buffer), 0);
        if (bytesReceived <= 0) {
            std::cout << "Client disconnected." << std::endl;
            break;
        }

        std::cout << "Received: " << buffer << std::endl;

        std::wstringstream wss;
        std::string response;

        if (strcmp(buffer, "list app") == 0) {
            std::vector<ProcessInfo> processes = ListApplications();
            std::map<ProcessType, std::vector<ProcessInfo>> groupedProcesses;

            for (const auto &process : processes) {
                groupedProcesses[process.type].push_back(process);
            }

            const wchar_t *typeNames[] = {L"Apps", L"Background processes", L"Windows processes"};

            for (int i = 0; i < 3; ++i) {
                ProcessType type = static_cast<ProcessType>(i);
                wss << typeNames[i] << L" (" << groupedProcesses[type].size() << L")\n";
                wss << std::wstring(50, L'-') << L"\n";

                for (const auto &process : groupedProcesses[type]) {
                    wss << std::left << std::setw(10) << process.pid << process.name << L"\n";
                }
                wss << L"\n";
            }
            wss << L"Total processes: " << processes.size() << L"\n";

        } else if (strcmp(buffer, "list service") == 0) {
            std::vector<ServiceInfo> services = ListServices();

            wss << std::left << std::setw(40) << L"Service Name"
                << std::setw(50) << L"Display Name"
                << L"State\n";
            wss << std::wstring(100, L'-') << L"\n";

            for (const auto &service : services) {
                wss << std::left << std::setw(40) << service.name
                    << std::setw(50) << service.displayName
                    << getStateString(service.currentState) << L"\n";
            }
            wss << L"\nTotal services: " << services.size() << L"\n";

        } else if (strcmp(buffer, "screen capture") == 0) {
            std::vector<char> imageData = ScreenCapture();
            wss << L"Screen capture completed.\n";
            // TODO: Generalize this
            // Convert wstring to string
            std::wstring wstr = wss.str();
            std::string str(wstr.begin(), wstr.end());

            // Send the response back to the client
            int bytesSent = send(clientSocket, str.c_str(), str.length(), 0);
            if (bytesSent == SOCKET_ERROR) {
                std::cerr << "send failed with error: " << WSAGetLastError() << std::endl;
                break;
            }

            // Send the image data back to the client
            send(clientSocket, imageData.data(), imageData.size(), 0);

            return;

        } else if (strcmp(buffer, "shutdown") == 0) {
            wss << L"Shutdown initiated. Shutting down after 15s\n";
            Shutdown();
        } else if (strcmp(buffer, "view file") == 0) {
            ViewFile();
            wss << L"File viewed.\n";
        } else if (strcmp(buffer, "get file") == 0) {
            GetFile();
            wss << L"File retrieved.\n";
        } else if (strcmp(buffer, "start webcam") == 0) {
            StartWebcam();
            wss << L"Webcam started.\n"; // TODO: Multithread
        } else {
            wss << L"Unknown command.\n";
        }

        if (strcmp(buffer, "exit") == 0) {
            std::cout << "Client requested to exit." << std::endl;
            break;
        }

        // Convert wstring to string
        std::wstring wstr = wss.str();
        std::string str(wstr.begin(), wstr.end());

        // Send the response back to the client
        int bytesSent = send(clientSocket, str.c_str(), str.length(), 0);
        if (bytesSent == SOCKET_ERROR) {
            std::cerr << "send failed with error: " << WSAGetLastError() << std::endl;
            break;
        }
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

void Server::StartListening() {
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

std::vector<ProcessInfo> Server::ListApplications() {
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


std::vector<ServiceInfo> Server::ListServices() {
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

void GetDesktopResolution(int& horizontal, int& vertical)
{
    RECT desktop;
    // Get a handle to the desktop window
    const HWND hDesktop = GetDesktopWindow();
    // Get the size of screen to the variable desktop
    GetWindowRect(hDesktop, &desktop);
    // The top left corner will have coordinates (0,0)
    // and the bottom right corner will have coordinates
    // (horizontal, vertical)
    horizontal = desktop.right;
    vertical = desktop.bottom;
}

std::vector<char> Server::ScreenCapture() {
    Gdiplus::GdiplusStartupInput gdiplusStartupInput;
    ULONG_PTR gdiplusToken;
    Gdiplus::GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

    int x1, y1, x2, y2, w, h;
    x1 = GetSystemMetrics(SM_XVIRTUALSCREEN);
    y1 = GetSystemMetrics(SM_YVIRTUALSCREEN);
    x2 = GetSystemMetrics(SM_CXVIRTUALSCREEN);
    y2 = GetSystemMetrics(SM_CYVIRTUALSCREEN);
    w = x2 - x1;
    h = y2 - y1;
    float ratio = 1.5; // TODO: Dynamic ratio
    w = int((float)w / ratio);
    h = int((float)h / ratio);

    HDC hScreen = GetDC(NULL);
    HDC hDC = CreateCompatibleDC(hScreen);
    HBITMAP hBitmap = CreateCompatibleBitmap(hScreen, w, h);
    HGDIOBJ old_obj = SelectObject(hDC, hBitmap);
    SetStretchBltMode(hDC, HALFTONE);
    StretchBlt(hDC, 0, 0, w, h, hScreen, x1, y1, x2 - x1, y2 - y1, SRCCOPY);

    Gdiplus::Bitmap* bitmap = Gdiplus::Bitmap::FromHBITMAP(hBitmap, NULL);

    CLSID jpegClsid;
    GetEncoderClsid(L"image/jpeg", &jpegClsid);

    IStream* istream = NULL;
    CreateStreamOnHGlobal(NULL, TRUE, &istream);

    Gdiplus::EncoderParameters encoderParameters;
    ULONG quality = 75;
    encoderParameters.Count = 1;
    encoderParameters.Parameter[0].Guid = Gdiplus::EncoderQuality;
    encoderParameters.Parameter[0].Type = Gdiplus::EncoderParameterValueTypeLong;
    encoderParameters.Parameter[0].NumberOfValues = 1;
    encoderParameters.Parameter[0].Value = &quality;

    bitmap->Save(istream, &jpegClsid, &encoderParameters);

    HGLOBAL hg = NULL;
    GetHGlobalFromStream(istream, &hg);
    int bufsize = GlobalSize(hg);
    std::vector<char> buffer(bufsize);

    LPVOID ptr = GlobalLock(hg);
    memcpy(&buffer[0], ptr, bufsize);
    GlobalUnlock(hg);

    istream->Release();
    delete bitmap;

    SelectObject(hDC, old_obj);
    DeleteDC(hDC);
    ReleaseDC(NULL, hScreen);
    DeleteObject(hBitmap);

    Gdiplus::GdiplusShutdown(gdiplusToken);

    return buffer;
}

void Server::Shutdown() {
    UINT nSDType = 0;
    HANDLE hToken;
    TOKEN_PRIVILEGES tkp;

    ::OpenProcessToken(::GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken);
    ::LookupPrivilegeValue(NULL, SE_SHUTDOWN_NAME, &tkp.Privileges[0].Luid);

    tkp.PrivilegeCount = 1; // set 1 privilege
    tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;

    // get the shutdown privilege for this process
    ::AdjustTokenPrivileges(hToken, FALSE, &tkp, 0, (PTOKEN_PRIVILEGES) NULL, 0);

    switch (nSDType) {
        case 0: ::ExitWindowsEx(EWX_SHUTDOWN | EWX_FORCE, 0);
            break;
        case 1: ::ExitWindowsEx(EWX_POWEROFF | EWX_FORCE, 0);
            break;
        case 2: ::ExitWindowsEx(EWX_REBOOT | EWX_FORCE, 0);
            break;
    }
};

void Server::ViewFile() {
    return;
}

void Server::StartWebcam() {
    WebcamController controller;
    controller.StartWebcam();

    //
    // HRESULT hr = controller.CaptureImage();
    // if (SUCCEEDED(hr)) {
    //     std::cout << "Screenshot captured successfully!" << std::endl;
    // } else {
    //     std::cout << "Failed to capture screenshot. Error code: " << hr << std::endl;
    // }
    //
    // controller.CleanUp();
};

void Server::GetFile() {
    return;
};
