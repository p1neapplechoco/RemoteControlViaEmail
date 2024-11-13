#include "Server.h"
#include "GetWinDirectory.h"
#include "IpDiscovery.h"

using namespace std;
// asio::io_context io_context;
// udp::socket multicast_socket(io_context);

bool endProcess(int appId)
{
    // Get a handle to the process
    HANDLE hProcess = OpenProcess(PROCESS_TERMINATE, FALSE, appId);

    if (hProcess == NULL)
    {
        std::cerr << "Failed to open process with ID " << appId << ". Error: " << GetLastError() << std::endl;
        return false;
    }

    // Terminate the process
    if (TerminateProcess(hProcess, 0))
    {
        std::cout << "Process with ID " << appId << " terminated successfully." << std::endl;
    } else
    {
        std::cerr << "Failed to terminate process with ID " << appId << ". Error: " << GetLastError() << std::endl;
    }
    // Close the process handle
    CloseHandle(hProcess);
    return true;
}

bool stopService(const char *serviceName)
{
    // Open the service control manager
    SC_HANDLE hSCManager = OpenSCManager(NULL, NULL, SC_MANAGER_CONNECT);
    if (hSCManager == NULL)
    {
        std::cerr << "Failed to open service control manager. Error: " << GetLastError() << std::endl;
        return false;
    }

    // Open the service
    SC_HANDLE hService = OpenService(hSCManager, serviceName, SERVICE_STOP | SERVICE_QUERY_STATUS);
    if (hService == NULL)
    {
        std::cerr << "Failed to open service: " << serviceName << ". Error: " << GetLastError() << std::endl;
        CloseServiceHandle(hSCManager);
        return false;
    }

    // Attempt to stop the service
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
    } else
    {
        std::cerr << "ControlService failed. Error: " << GetLastError() << std::endl;
    }

    // Close the service and service control manager handles
    CloseServiceHandle(hService);
    CloseServiceHandle(hSCManager);
    return true;
}

// Helper function to get the encoder CLSID for a given image format
int GetEncoderClsid(const WCHAR *format, CLSID *pClsid)
{
    UINT num = 0;
    UINT size = 0;

    Gdiplus::ImageCodecInfo *pImageCodecInfo = NULL;

    Gdiplus::GetImageEncodersSize(&num, &size);
    if (size == 0)
        return -1;

    pImageCodecInfo = (Gdiplus::ImageCodecInfo *) (malloc(size));
    if (pImageCodecInfo == NULL)
        return -1;

    Gdiplus::GetImageEncoders(num, size, pImageCodecInfo);

    for (UINT j = 0; j < num; ++j)
    {
        if (wcscmp(pImageCodecInfo[j].MimeType, format) == 0)
        {
            *pClsid = pImageCodecInfo[j].Clsid;
            free(pImageCodecInfo);
            return j;
        }
    }

    free(pImageCodecInfo);
    return -1;
}

// for display purpose
std::wstring getStateString(DWORD state)
{
    switch (state)
    {
        case SERVICE_STOPPED:
            return L"Stopped";
        case SERVICE_START_PENDING:
            return L"Start Pending";
        case SERVICE_STOP_PENDING:
            return L"Stop Pending";
        case SERVICE_RUNNING:
            return L"Running";
        case SERVICE_CONTINUE_PENDING:
            return L"Continue Pending";
        case SERVICE_PAUSE_PENDING:
            return L"Pause Pending";
        case SERVICE_PAUSED:
            return L"Paused";
        default:
            return L"Unknown";
    }
}

pair<int, int> GetPhysicalDesktopDimensions()
{
    DEVMODE devMode;
    devMode.dmSize = sizeof(DEVMODE);
    EnumDisplaySettings(nullptr, ENUM_CURRENT_SETTINGS, &devMode);
    int screenWidth = devMode.dmPelsWidth;
    int screenHeight = devMode.dmPelsHeight;
    return {screenWidth, screenHeight};
}

#pragma comment(lib, "ws2_32.lib")

void Server::handleClient(SOCKET clientSocket)
{
    char buffer[1024];
    while (true)
    {
        memset(buffer, 0, sizeof(buffer));
        int bytesReceived = recv(clientSocket, buffer, sizeof(buffer), 0);
        if (bytesReceived <= 0)
        {
            std::cout << "Client disconnected." << std::endl;
            break;
        }

        std::cout << "Received: " << buffer << std::endl;

        std::wstringstream wss;
        std::string response;

        if (strcmp(buffer, "list app") == 0)
        {
            std::vector<ProcessInfo> processes = ListApplications();
            std::map<ProcessType, std::vector<ProcessInfo> > groupedProcesses;

            for (const auto &process: processes)
            {
                groupedProcesses[process.type].push_back(process);
            }

            const wchar_t *typeNames[] = {L"Apps", L"Background processes", L"Windows processes"};

            for (int i = 0; i < 3; ++i)
            {
                ProcessType type = static_cast<ProcessType>(i);
                wss << typeNames[i] << L" (" << groupedProcesses[type].size() << L")\n";
                wss << std::wstring(50, L'-') << L"\n";

                for (const auto &process: groupedProcesses[type])
                {
                    wss << std::left << std::setw(10) << process.pid << process.name << L"\n";
                }
                wss << L"\n";
            }
            wss << L"Total processes: " << processes.size() << L"\n";
        } else if (strcmp(buffer, "list service") == 0)
        {
            std::vector<ServiceInfo> services = ListServices();

            wss << std::left << std::setw(40) << L"Service Name"
                    << std::setw(50) << L"Display Name"
                    << L"State\n";
            wss << std::wstring(100, L'-') << L"\n";

            for (const auto &service: services)
            {
                wss << std::left << std::setw(40) << service.name
                        << std::setw(50) << service.displayName
                        << getStateString(service.currentState) << L"\n";
            }
            wss << L"\nTotal services: " << services.size() << L"\n";
        } else if (strcmp(buffer, "screen capture") == 0)
        {
            imageData = ScreenCapture();
            wss << L"Screen capture completed.\n";
            // TODO: Generalize this
        } else if (strcmp(buffer, "shutdown") == 0)
        {
            wss << L"Shutdown initiated. Shutting down after 15s\n";
            Shutdown();
        } else if (strcmp(buffer, "view file") == 0)
        {
            ViewFile();
            wss << L"File viewed.\n";
        } else if (strcmp(buffer, "get file") == 0)
        {
            GetFile();
            wss << L"File retrieved.\n";
        } else if (strcmp(buffer, "start webcam") == 0)
        {
            StartWebcam();
            wss << L"Webcam started.\n"; // TODO: Multithread
        } else if (strstr(buffer, "enda ") != nullptr)
        {
            char *endPtr;
            int app_id = strtol(buffer + 4, &endPtr, 10);
            if (*endPtr == '\0' || *endPtr == ' ')
            {
                std::cout << app_id << std::endl;
                wss << "Trying to close " << app_id << std::endl;
                if (!endProcess(app_id))
                {
                    wss << "No such app with such ID" << std::endl;
                }
            } else
                wss << "Invalid ID" << std::endl;
        } else if (strstr(buffer, "ends ") == buffer)
        {
            char serviceName[256] = {0}; // Ensure the array is zero-initialized
            char *start = buffer + 5; // Start after "ends "

            // Copy the service name into serviceName and remove trailing newlines and spaces
            size_t len = 0;
            while (start[len] != '\0' && start[len] != '\n' && start[len] != ' ' && start[len] != '\r' && len < sizeof(serviceName) - 1)
            {
                serviceName[len] = start[len];
                len++;
            }
            serviceName[len] = '\0'; // Null-terminate the string

            wss << L"Trying to stop service: " << serviceName << std::endl;
            if (!stopService(serviceName))
            {
                wss << L"Failed to stop the service." << std::endl;
            }
        } else if (strcmp(buffer, "stop webcam") == 0)
        {
            StopWebcam();
            wss << L"Webcam stopped.\n"; // TODO: Multithread
        } else if (strcmp(buffer, "get webcam frame") == 0)
        {
            wss << L"Webcam frame sent.\n";
        } else
        {
            wss << L"Unknown command.\n";
        }

        if (strcmp(buffer, "exit") == 0)
        {
            std::cout << "Client requested to exit." << std::endl;
            break;
        }

        // Convert wstring to string
        std::wstring wstr = wss.str();
        std::string str(wstr.begin(), wstr.end());

        // Send the response back to the client
        // Send the size of str first
        // Send the response back to the client
        // Send the size of str first
        int responseSize = str.length();
        send(clientSocket, reinterpret_cast<char *>(&responseSize), sizeof(int), 0);

        // Send the actual str
        int bytesSent = send(clientSocket, str.c_str(), str.length(), 0);
        if (bytesSent == SOCKET_ERROR)
        {
            std::cerr << "send failed with error: " << WSAGetLastError() << std::endl;
            break;
        }

        if (strcmp(buffer, "screen capture") == 0)
        {
            // Send the size of the image data first
            int imageSize = static_cast<int>(imageData.size());
            send(clientSocket, reinterpret_cast<char *>(&imageSize), sizeof(int), 0);

            // Send the actual image data
            bytesSent = send(clientSocket, imageData.data(), imageData.size(), 0);
            if (bytesSent == SOCKET_ERROR)
            {
                std::cerr << "send failed with error: " << WSAGetLastError() << std::endl;
                break;
            }
            std::cout << "Sent image data of size: " << imageSize << " bytes" << std::endl;
        } else if (strcmp(buffer, "get webcam frame") == 0)
        {
            std::vector<char> frameData = GetWebcamFrame();

            // Send frame size
            int frameSize = frameData.size();
            send(clientSocket, reinterpret_cast<char *>(&frameSize), sizeof(int), 0);

            // Send frame data
            if (!frameData.empty())
            {
                send(clientSocket, frameData.data(), frameData.size(), 0);
                std::cout << "Sent webcam frame of size: " << frameSize << " bytes" << std::endl;
            }
        }
    }
    closesocket(clientSocket);
}

Server::Server()
{
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
    {
        std::cerr << "Failed to initialize Winsock" << std::endl;
        return;
    }

    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == INVALID_SOCKET)
    {
        std::cerr << "Failed to create socket" << std::endl;
        WSACleanup();
        return;
    }
    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = 0;

    if (bind(server_socket, (sockaddr *) &serverAddr, sizeof(serverAddr)) == SOCKET_ERROR)
    {
        std::cerr << "Bind failed" << std::endl;
        closesocket(server_socket);
        WSACleanup();
        return;
    }

    // Get the assigned port
    int addrLen = sizeof(serverAddr);
    if (getsockname(server_socket, (sockaddr *) &serverAddr, &addrLen) == SOCKET_ERROR)
    {
        cerr << "Failed to get socket name" << endl;
        closesocket(server_socket);
        WSACleanup();
        return;
    }
    server_port = ntohs(serverAddr.sin_port);
}

Server::~Server()
{
    closesocket(server_socket);
    WSACleanup();
    cout << "Server destroyed" << endl;
}

void Server::StartListening()
{
    if (listen(server_socket, SOMAXCONN) == SOCKET_ERROR)
    {
        std::cerr << "Listen failed" << std::endl;
        closesocket(server_socket);
        WSACleanup();
        return;
    }
    cout << "Server listening for discovery on port " << server_port << std::endl;

    // Wait for a connection, Default when enter

    while (true)
    {
        DiscoveryResponder responder;
        responder.listen();

        // Receive and send messages
        while (true)
        {
            SOCKET clientSocket = accept(server_socket, NULL, NULL);
            if (clientSocket == INVALID_SOCKET)
            {
                cerr << "Accept failed" << endl;
                continue;
            }
            cout << "New client connected." << endl;
            handleClient(clientSocket);
            break;
        }
    }
}

std::vector<ProcessInfo> Server::ListApplications()
{
    std::vector<ProcessInfo> processes;
    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hSnapshot == INVALID_HANDLE_VALUE)
    {
        throw std::runtime_error("CreateToolhelp32Snapshot failed");
    }

    PROCESSENTRY32W pe32;
    pe32.dwSize = sizeof(PROCESSENTRY32W);

    if (!Process32FirstW(hSnapshot, &pe32))
    {
        CloseHandle(hSnapshot);
        throw std::runtime_error("Process32First failed");
    }

    do
    {
        HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, pe32.th32ProcessID);
        if (hProcess)
        {
            wchar_t szProcessPath[MAX_PATH];
            if (GetModuleFileNameExW(hProcess, NULL, szProcessPath, MAX_PATH))
            {
                ProcessType type;
                if (wcsstr(szProcessPath, L"\\Windows\\") != nullptr)
                {
                    type = ProcessType::WindowsProcess;
                } else if (pe32.cntThreads > 1)
                {
                    type = ProcessType::App;
                } else
                {
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

std::vector<ServiceInfo> Server::ListServices()
{
    std::vector<ServiceInfo> services;

    SC_HANDLE hSCManager = OpenSCManagerW(nullptr, nullptr, SC_MANAGER_ENUMERATE_SERVICE);
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

std::vector<char> Server::ScreenCapture()
{
    Gdiplus::GdiplusStartupInput gdiplusStartupInput;
    ULONG_PTR gdiplusToken;
    Gdiplus::GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

    const int x1 = GetSystemMetrics(SM_XVIRTUALSCREEN);
    const int y1 = GetSystemMetrics(SM_YVIRTUALSCREEN);
    int x2 = GetSystemMetrics(SM_CXVIRTUALSCREEN);
    int y2 = GetSystemMetrics(SM_CYVIRTUALSCREEN);
    const pair<int, int> resolution = GetPhysicalDesktopDimensions();
    int w = resolution.first;
    int h = resolution.second;
    constexpr float ratio = 1.0f; // TODO: Dynamic ratio
    w = static_cast<int>(static_cast<float>(w) * ratio);
    h = static_cast<int>(static_cast<float>(h) * ratio);
    const HDC hScreen = GetDC(nullptr);
    const HDC hDC = CreateCompatibleDC(hScreen);
    HBITMAP hBitmap = CreateCompatibleBitmap(hScreen, w, h);
    HGDIOBJ old_obj = SelectObject(hDC, hBitmap);
    SetStretchBltMode(hDC, HALFTONE);
    StretchBlt(hDC, 0, 0, w, h, hScreen, x1, y1, w, h, SRCCOPY);

    Gdiplus::Bitmap *bitmap = Gdiplus::Bitmap::FromHBITMAP(hBitmap, nullptr);

    CLSID jpegClsid;
    GetEncoderClsid(L"image/jpeg", &jpegClsid);

    IStream *istream = nullptr;
    CreateStreamOnHGlobal(nullptr, TRUE, &istream);

    Gdiplus::EncoderParameters encoderParameters;
    ULONG quality = 150;
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

void Server::Shutdown()
{
    UINT nSDType = 0;
    HANDLE hToken;
    TOKEN_PRIVILEGES tkp;

    ::OpenProcessToken(::GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken);
    ::LookupPrivilegeValue(NULL, SE_SHUTDOWN_NAME, &tkp.Privileges[0].Luid);

    tkp.PrivilegeCount = 1; // set 1 privilege
    tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;

    // get the shutdown privilege for this process
    ::AdjustTokenPrivileges(hToken, FALSE, &tkp, 0, (PTOKEN_PRIVILEGES) NULL, 0);

    switch (nSDType)
    {
        case 0:
            ::ExitWindowsEx(EWX_SHUTDOWN | EWX_FORCE, 0);
            break;
        case 1:
            ::ExitWindowsEx(EWX_POWEROFF | EWX_FORCE, 0);
            break;
        case 2:
            ::ExitWindowsEx(EWX_REBOOT | EWX_FORCE, 0);
            break;
    }
};

void Server::StartWebcam()
{
    controller.StartWebcam();
};

void Server::StopWebcam()
{
    controller.StopWebcam();
};

void Server::ViewFile()
{
    return;
}

void Server::GetFile()
{
    return;
};

std::vector<char> Server::GetWebcamFrame()
{
    return controller.GetCurrentFrame();
}

// Function to start an application and return its PID
DWORD StartApplication(const std::wstring &applicationPath)
{
    STARTUPINFO si = {sizeof(si)};
    PROCESS_INFORMATION pi;

    if (CreateProcess(reinterpret_cast<LPCSTR>(applicationPath.c_str()), NULL, NULL, NULL, FALSE, 0, NULL, NULL, &si,
                      &pi))
    {
        CloseHandle(pi.hThread);
        CloseHandle(pi.hProcess);
        return pi.dwProcessId;
    }

    return 0; // Return 0 if process creation failed
}

bool StopApplication(DWORD pid)
{
    HANDLE hProcess = OpenProcess(PROCESS_TERMINATE, FALSE, pid);
    if (hProcess == NULL)
    {
        return false;
    }

    bool result = TerminateProcess(hProcess, 0);
    CloseHandle(hProcess);
    return result;
}