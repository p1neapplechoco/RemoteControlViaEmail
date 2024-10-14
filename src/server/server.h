//
// Created by phida on 10/9/2024.
//
#include <winsock2.h>
#include <windows.h>
#include <winsvc.h>
#include <psapi.h>
#include <tlhelp32.h>
#include <vector>
#include <string>
#include <map>
#include <algorithm>
#include <iostream>
#include <iomanip>

#include "WebcamController.h"


#pragma once
#pragma comment(lib, "ws2_32.lib")

#ifndef SERVER_H
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


enum class ProcessType {
    App,
    BackgroundProcess,
    WindowsProcess
};

struct ServiceInfo {
    std::wstring name;
    std::wstring displayName;
    DWORD currentState;
};

struct ProcessInfo {
    DWORD pid;
    std::wstring name;
    ProcessType type;
};

class Server {
private:
    int assignedPort;
    SOCKET serverSocket;

    void handleClient(SOCKET);

public:
    Server();

    ~Server();

    void StartListening();


    // Dynamically
    std::vector<ProcessInfo> ListApplications();

    std::vector<ServiceInfo> ListServices();

    void ScreenCapture();

    void Shutdown();

    void ViewFile();

    void StartWebcam();

    void GetFile();
};
