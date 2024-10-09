//
// Created by phida on 10/9/2024.
//

#include <iostream>
#include <winsock2.h>
#include <stdio.h>
#pragma comment(lib, "ws2_32.lib")

int main() {
    /*
    1. Creating the Client Socket
    2. Connecting to the Server
    3. Sending Data to the Server
     */

    // Đoạn này handle exception cho vui
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "Failed to initialize Winsock" << std::endl;
        return 1;
    }

    SOCKET clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket == INVALID_SOCKET) {
        std::cerr << "Failed to create socket" << std::endl;
        WSACleanup();
        return 1;
    }

    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1"); // parametrize this
    serverAddr.sin_port = htons(8080); // parametrize this

    // Connect to the server
    SOCKET serverSocket = accept(clientSocket, NULL, NULL);
    if (connect(clientSocket, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        std::cerr << "Connection failed" << std::endl;
        closesocket(clientSocket);
        WSACleanup();
        return 1;
    }

    while (true) {
        char buffer[1024] = {};
        std::cout << "Enter message: ";
        std::cin.getline(buffer, sizeof(buffer));
        send(clientSocket, buffer, sizeof(buffer), 0);
        // clear buffer
        // client send-only
        // must make listen after
        // memset(buffer, 0, sizeof(buffer));
        // recv(clientSocket, buffer, sizeof(buffer), 0);
        // std::cout << "Server response: " << buffer << std::endl;
        if (strcmp(buffer, "exit") == 0) {
            break;
        }
    }
    // Clean-up
    closesocket(clientSocket);
    WSACleanup();
    return 0;
}