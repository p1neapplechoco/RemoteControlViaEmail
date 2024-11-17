#pragma once
#pragma comment(lib, "ws2_32.lib")

#ifndef SERVER_H

#include <algorithm>
#include <assert.h>
#include <chrono>
#include <fcntl.h>
#include <functional>
#include <gdiplus.h>
#include <io.h>
#include <iomanip>
#include <iostream>
#include <map>
#include <psapi.h>
#include <sstream>
#include <string>
#include <thread>
#include <tlhelp32.h>
#include <vector>
#include <windows.h>
#include <windows.h>
#include <winsock2.h>
#include <winsvc.h>
#include <ws2tcpip.h>
#include "WebcamController.h"

#define SERVER_H

#endif //SERVER_H

// Design structure
/*
1. Creating the Server Socket || Server inputs: IP, port number
2. Listen for connection on all ports
3. Get the commands from the client
4. Fetching data - do actions
5. Send the response back to the client
6. Don't quit
 */


enum class ProcessType
{
    App,
    BackgroundProcess,
    WindowsProcess
};

struct ServiceInfo
{
    std::wstring name;
    std::wstring displayName;
    DWORD currentState;
};

struct ProcessInfo
{
    DWORD pid;
    std::wstring name;
    ProcessType type;
};

class Server
{
private:
    int default_port = 45678;
    int server_port;
    SOCKET server_socket;
    WebcamController webcam_controller;

    void handleClient(SOCKET);

public:
    std::vector<char> imageData;

    Server();

    ~Server();

    void StartListening();


    // Dynamically
    std::vector<ProcessInfo> ListApplications();

    std::vector<ServiceInfo> ListServices();

    std::vector<char> ScreenCapture();

    void Shutdown(UINT);

    void StartWebcam();

    void StopWebcam();

    std::vector<char> GetWebcamFrame();

    void ViewFile();

    void GetFile();
};
