#pragma once
#pragma comment(lib, "ws2_32.lib")

#ifndef SERVER_H

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
#include "discoveryResponder.h"
#include "Process.h"
#include "Service.h"
#include "WindowsCommands.h"

#include "../utils/GetWinDirectory.h"

#define LISTEN_TIMEOUT_SECONDS 100; // Adjust timeout as needed
#define CONNECTION_TIMEOUT_SECONDS 100;


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


class Server {
private:
    int default_port = 42069;
    int assigned_port{};
    std::wstringstream wss{};

    WSADATA wsa_data{};
    SOCKET server_socket{};
    sockaddr_in server_address{};
    WebcamController webcam_controller{};
    GetWinDirectory getWinDir{};
    void handleClient(SOCKET);

public:


    Server();

    ~Server();

    // setting up

    bool setupWSA();

    bool setupSocket();

    bool assignPort();

    bool setupServer();

    void startServer();

    // commands handler

    // [COMMANDS]
    void listOfCommands();

    int sendSizeAndResponse(const SOCKET &client_socket) const;

    // [Application/ Services Commands]
    void listProcesses();

    void listServices();

    void endProcess(const char *buffer);

    void startService(const char *buffer);

    void endService(const char *buffer);

    // [Webcam/ Screenshot Commands]
    void screenShot(std::vector<char> &image);

    void toggleWebcam();

    void capture(vector<char> &image);

    // [Windows Explorer Commands]
    void IndexSystem(string, SOCKET);

    void GetAndSendFile(string, SOCKET);

    void SendImage(vector<char> &image, SOCKET);

    void ShowAvailableDisks();

    void Shutdown(const char *buffer);

    // void openAndSendFile(string file_path);
    // void StartListening();
    // void OpenFile(string);
};
