#pragma once
#ifndef NETWORKDISCOVERY_H

#include <iostream>
#include <string>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <vector>
#include <string>
using namespace std;

#define NETWORKDISCOVERY_H

class NetworkDiscovery
{
private:
    SOCKET broadcast_socket;
    const int PORT = 12345;
    const string DISCOVERY_MESSAGE = "DISCOVER_REQUEST";
    const string RESPOND_MESSAGE = "DISCOVER_RESPONSE";
    vector<string> discoveredIPs;

public:
    NetworkDiscovery();

    ~NetworkDiscovery();

    void listenForDiscovery();

    void sendBroadcast() const;

    void listenForResponses(int timeout_second = 5);

    vector<string> getDiscoveredIPs() const { return discoveredIPs; }
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
