#pragma once
#pragma comment(lib, "ws2_32.lib")

#include <iomanip>
#include <sstream>
#include <vector>
#include <winsock2.h>
#include "WebcamController.h"
#include <dirent.h>
#include <functional>
#include <gdiplus.h>
#include <io.h>
#include <iostream>
#include <map>
#include <psapi.h>
#include <string>
#include <thread>
#include <unistd.h>
#include <windows.h>
#include <ws2tcpip.h>
#include <sys/types.h>
#include "IpDiscovery.h"
#include "Process.h"
#include "Service.h"
#include "WindowsCommands.h"
#include "GetWinDirectory.h"

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


class Server
{
private:
    int default_port = 45678;
    int assigned_port{};
    std::wstringstream wss{};

    WSADATA wsa_data{};
    SOCKET server_socket{};
    sockaddr_in server_address{};
    WebcamController webcam_controller{};

    void handleClient(SOCKET);

public:
    Server();

    ~Server();

    // setting up

    bool setupWSA();

    bool setupSocket();

    bool assignPort();

    bool setupServer();

    // commands handler

    void listOfCommands();

    void listProcesses();

    void listServices();

    void screenShot(std::vector<char> &image);

    void toggleWebcam();

    void shutdown(const char *buffer);

    void endProcess(const char *buffer);

    void endService(const char *buffer);

    void capture(vector<char> &image);

    int sendSizeAndResponse(const SOCKET &client_socket) const;

    void startServer();

    // void StartListening();
};
