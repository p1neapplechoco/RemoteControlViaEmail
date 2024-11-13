#pragma once
#ifndef CLIENT_H

#include <string>
#include <vector>
#include <winsock2.h>
#include "EmailRetrieval.h"

#define CLIENT_H

class Client
{
private:
    EmailRetrieval email_retrieval;
    std::string server_ip = "127.0.0.1";
    int server_port = 0;

public:
    WSADATA wsaData{};
    SOCKET client_socket{};
    sockaddr_in server_address{};

    Client();

    ~Client();

    bool setupWSA();

    bool setupSocket();

    bool connectToServer();

    bool setupClient();

    static void scanIP();

    std::vector<char> receiveImageData() const;

    void startClient();
};

#endif //CLIENT_H
