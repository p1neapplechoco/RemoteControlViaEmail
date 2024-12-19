//
// Created by phida on 11/6/2024.
//

#include "discoveryResponder.h"

DiscoveryResponder::DiscoveryResponder() {
    // Initialize Winsock
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        throw std::runtime_error("WSAStartup failed");
    }

    // Create UDP socket
    serverSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (serverSocket == INVALID_SOCKET) {
        WSACleanup();
        throw std::runtime_error("Socket creation failed");
    }

    // Bind socket
    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(PORT);
    serverAddr.sin_addr.s_addr = INADDR_ANY;

    if (bind(serverSocket, (sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
        closesocket(serverSocket);
        WSACleanup();
        throw std::runtime_error("Bind failed");
    }
}

DiscoveryResponder::~DiscoveryResponder() {
    closesocket(serverSocket);
    WSACleanup();
}

void DiscoveryResponder::listen() {
    char buffer[1024];
    sockaddr_in senderAddr;
    int senderAddrLen = sizeof(senderAddr);

    std::cout << "Listening for discovery requests...\n";

    while (true) {
        std::cout << "skibidi" << std::endl;
        int bytesReceived = recvfrom(serverSocket, buffer, sizeof(buffer), 0,
                                   (sockaddr*)&senderAddr, &senderAddrLen); // Waiting

        std::cout << bytesReceived << std::endl;
        if (bytesReceived < 0) {
            throw std::runtime_error("Error receiving request");
        }
        buffer[bytesReceived] = '\0';
        if (std::string(buffer) == "DISCOVER_REQUEST") {
            // Send response
            if (sendto(serverSocket, RESPONSE_MESSAGE.c_str(),
                      RESPONSE_MESSAGE.length(), 0,
                      (sockaddr*)&senderAddr, senderAddrLen) < 0)
            {
                throw std::runtime_error("Failed to send response");
            }
            break;
        }
    }
}