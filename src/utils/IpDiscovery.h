#include <iostream>
#include <string>
#include <vector>
#include <WinSock2.h>
#include <WS2tcpip.h>
using namespace std;

#ifndef NETWORKDISCOVERY_H
#define NETWORKDISCOVERY_H

class NetworkDiscovery
{
private:
    SOCKET broadcast_socket;
    const int PORT = 12345;
    const std::string DISCOVERY_MESSAGE = "DISCOVER_REQUEST";
    const std::string RESPOND_MESSAGE = "DISCOVER_RESPONSE";
    vector<string> discoveredIPs;

public:
    NetworkDiscovery();

    ~NetworkDiscovery();

    void listenForDiscovery();

    void sendBroadcast() const;

    void listenForResponses(int);

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
