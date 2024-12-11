#include <iostream>
#include <string>
#include <vector>
#include <WinSock2.h>
#include <WS2tcpip.h>
using namespace std;

#ifndef IPDISCOVERY_H
#define IPDISCOVERY_H

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
