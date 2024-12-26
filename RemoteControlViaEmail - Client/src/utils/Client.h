#pragma once
#ifndef CLIENT_H

#include <string>
#include <vector>
#include <winsock2.h>
#include "Email.h"

using namespace std;

#define CLIENT_H

class Client
{
private:
    EMAIL email_retrieval;
    const int PORT = 42069;

public:
    WSADATA wsaData{};
    SOCKET clientSocket{};
    sockaddr_in serverAddr;

    string email, password;

    Client();

    ~Client();

    bool setupClient();

    bool connectToServer(string serverIP);

    static vector<string> scanIP();

    std::vector<char> receiveImageData();

    void startClient();

    bool handleCommand(const string &command, string &reponseClient, string &filePath);
};

#endif // CLIENT_H
