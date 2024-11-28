//
// Created by phida on 11/6/2024.
//

#include <iostream>
#include <string>
#include <vector>
#include <WinSock2.h>
#include <WS2tcpip.h>

#ifndef NETWORKDISCOVERY_H
#define NETWORKDISCOVERY_H
class NetworkDiscovery {
private:
    SOCKET broadcastSocket;
    const int PORT = 42069;  // Choose an available port
    const std::string DISCOVERY_MESSAGE = "DISCOVER_REQUEST";
    std::vector<std::string> discoveredIPs;

public:
    NetworkDiscovery();

    ~NetworkDiscovery();

    void sendBroadcast();

    void listenForResponses(int);

    std::vector<std::string> getDiscoveredIPs() { return discoveredIPs; }
};
#endif //NETWORKDISCOVERY_H
