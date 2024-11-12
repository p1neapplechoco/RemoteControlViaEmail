//
// Created by phida on 11/6/2024.
//
#include <iostream>
#include <string>
#include <WinSock2.h>
#include <WS2tcpip.h>

#ifndef DISCOVERYRESPONDER_H
#define DISCOVERYRESPONDER_H

class DiscoveryResponder {
private:
    SOCKET serverSocket;
    const int PORT = 12345;
    const std::string RESPONSE_MESSAGE = "DISCOVERY_RESPONSE";

public:
    DiscoveryResponder();

    ~DiscoveryResponder();

    void listen();
};

#endif //DISCOVERYRESPONDER_H
