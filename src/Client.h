#pragma once
#ifndef CLIENT_H

#include "EmailRetrieval.h"
#include <iostream>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <string>
#include <vector>
#include <fstream>

#define CLIENT_H

class Client
{
public:
    WSADATA wsaData;
    SOCKET client_socket;

    bool setupWSA();

    bool setupSocket();
};

#endif //CLIENT_H
