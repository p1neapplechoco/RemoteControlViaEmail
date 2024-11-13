#pragma once
#ifndef NETWORKDISCOVERY_H

#include <iostream>
#include <string>
#include <WinSock2.h>
#include <WS2tcpip.h>

#define NETWORKDISCOVERY_H

class NetworkDiscovery
{
private:
    SOCKET broadcast_socket;
    const int PORT = 12345;
    const std::string DISCOVERY_MESSAGE = "DISCOVER_REQUEST";
    const std::string RESPOND_MESSAGE = "DISCOVER_RESPONSE";

public:
    NetworkDiscovery();

    ~NetworkDiscovery();

    void listenForDiscovery();

    void sendBroadcast() const;

    void listenForResponses(int) const;
};

class DiscoveryResponder
{
private
:
    SOCKET responder_socket;
    const int PORT = 12345;
    const std::string RESPONSE_MESSAGE = "DISCOVERY_RESPONSE";

public
:
    DiscoveryResponder();

    ~DiscoveryResponder();

    void listen() const;
};

#endif
