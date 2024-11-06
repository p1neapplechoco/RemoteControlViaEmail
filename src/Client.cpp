#include "Client.h"

bool Client::setupWSA()
{
    return WSAStartup(MAKEWORD(2, 2), &wsaData) == 0;
}

bool Client::setupSocket()
{
    client_socket = socket(AF_INET, SOCK_STREAM, 0);
    return client_socket != INVALID_SOCKET;
}

