//
// Created by phida on 11/6/2024.
//

#include "networkDiscovery.h"

NetworkDiscovery::NetworkDiscovery() {
    // Initialize Winsock
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        throw std::runtime_error("WSAStartup failed");
    }

    // Create UDP socket
    broadcastSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (broadcastSocket == INVALID_SOCKET) {
        WSACleanup();
        throw std::runtime_error("Socket creation failed");
    }

    // Enable broadcasting
    BOOL broadcastEnable = TRUE;
    if (setsockopt(broadcastSocket, SOL_SOCKET, SO_BROADCAST,
                   (char *) &broadcastEnable, sizeof(broadcastEnable)) < 0) {
        closesocket(broadcastSocket);
        WSACleanup();
        throw std::runtime_error("Failed to enable broadcasting");
    }
}

NetworkDiscovery::~NetworkDiscovery() {
    closesocket(broadcastSocket);
    WSACleanup();
}

void NetworkDiscovery::sendBroadcast() {
    sockaddr_in broadcastAddr;
    broadcastAddr.sin_family = AF_INET;
    broadcastAddr.sin_port = htons(PORT);
    broadcastAddr.sin_addr.s_addr = INADDR_BROADCAST;

    if (sendto(broadcastSocket, DISCOVERY_MESSAGE.c_str(),
               DISCOVERY_MESSAGE.length(), 0,
               (sockaddr *) &broadcastAddr, sizeof(broadcastAddr)) < 0) {
        throw std::runtime_error("Failed to send broadcast");
    }
}

void NetworkDiscovery::listenForResponses(int timeoutSeconds = 5) {
    // Set receive timeout
    discoveredIPs.clear();
    DWORD timeout = timeoutSeconds * 1000;
    if (setsockopt(broadcastSocket, SOL_SOCKET, SO_RCVTIMEO,
                   (char *) &timeout, sizeof(timeout)) < 0) {
        throw std::runtime_error("Failed to set timeout");
    }

    char buffer[1024];
    sockaddr_in senderAddr;
    int senderAddrLen = sizeof(senderAddr);

    std::cout << "Listening for responses...\n";

    while (true) {
        int bytesReceived = recvfrom(broadcastSocket, buffer, sizeof(buffer), 0,
                                     (sockaddr *) &senderAddr, &senderAddrLen);

        if (bytesReceived < 0) {
            if (WSAGetLastError() == WSAETIMEDOUT) {
                std::cout << "Discovery complete\n";
                break;
            }
            throw std::runtime_error("Error receiving response");
        }

        buffer[bytesReceived] = '\0';
        char senderIP[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &(senderAddr.sin_addr), senderIP, INET_ADDRSTRLEN);

        std::cout << "Response from " << senderIP << ": " << buffer << "\n";
        std::string ipStr(senderIP);
        discoveredIPs.push_back(ipStr);
    }
}
