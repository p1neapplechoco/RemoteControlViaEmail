//
// Created by phida on 11/6/2024.
//

#include <iostream>
#include <string>
#include <WinSock2.h>
#include <WS2tcpip.h>

#ifndef NETWORKDISCOVERY_H
#define NETWORKDISCOVERY_H
class NetworkDiscovery {
private:
    SOCKET broadcastSocket;
    const int PORT = 12345;  // Choose an available port
    const std::string DISCOVERY_MESSAGE = "DISCOVER_REQUEST";
public:
    NetworkDiscovery();
    ~NetworkDiscovery();
    void sendBroadcast();
    void listenForResponses(int);
};
#endif //NETWORKDISCOVERY_H
