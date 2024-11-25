#pragma once
#ifndef CLIENT_H

#include <string>
#include <vector>
#include <winsock2.h>
#include "EmailRetrieval.h"
using namespace std;

#define CLIENT_H

class Client
{
private:
    EmailRetrieval email_retrieval;
    string server_ip = "127.0.0.1";
    int server_port = 0;

public:
    WSADATA wsa_data{};
    SOCKET client_socket{};
    sockaddr_in server_address{};

    Client();

    ~Client();

    void setServerPort(const long &port) { server_port = port; }
    void setServerIP(const string &serverAddress) { server_ip = serverAddress; }

    bool setupWSA();

    bool setupSocket();

    bool setupClient();

    bool connectToServer();

    static vector<string> scanIP();

    std::vector<char> receiveImageData() const;

    void startClient();

    bool handleCommand(const std::string& command);
};

#endif //CLIENT_H
