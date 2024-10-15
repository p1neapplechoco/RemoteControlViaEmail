#include "EmailRetrieval.h"
#include <io.h>
#include <iostream>
#include <winsock2.h>
#include <stdio.h>

void removeCarriageReturns(char* str) {
    char* write = str;
    for (char* read = str; *read; read++) {
        if (*read != '\r') {
            *write++ = *read;
        }
    }
    *write = '\0';
}

int main() {
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "Failed to initialize Winsock" << std::endl;
        return 1;
    }

    SOCKET clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket == INVALID_SOCKET) {
        std::cerr << "Failed to create socket" << std::endl;
        WSACleanup();
        return 1;
    }

    std::string serverIP;
    int serverPort;

    std::cout << "Enter server IP address: ";
    std::cin >> serverIP;
    std::cout << "Enter server port: ";
    std::cin >> serverPort;
    std::cin.ignore(); // Clear the newline from the input buffer

    sockaddr_in serverAddr{};
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = inet_addr(serverIP.c_str()); // parametrize this
    serverAddr.sin_port = htons(serverPort); // parametrize this

    // Connect to the server
    if (connect(clientSocket, reinterpret_cast<sockaddr *>(&serverAddr), sizeof(serverAddr)) == SOCKET_ERROR) {
        std::cerr << "Connection failed" << std::endl;
        closesocket(clientSocket);
        WSACleanup();
        return 1;
    }

    std::cout << "Connected to server at " << serverIP << ":" << serverPort << std::endl;

    // Setup user
    UserCredentials user;
    user.loadCredentials();
    EmailRetrieval emailRetrieval(user);

    emailRetrieval.setupCurl();

    while (true) {
        char sent_buffer[1024] = {};
        std::string prev_mail_id = " ";
        bool fl = true; // Ignore the first email

        while (true) {
            emailRetrieval.retrieveEmail();
            if (fl) {
                prev_mail_id = emailRetrieval.getMailID();
                fl = false;
            }
            if (prev_mail_id != emailRetrieval.getMailID()) {
                std::string str = emailRetrieval.getMailContent();
                strcpy(sent_buffer, emailRetrieval.getMailContent().c_str());
                removeCarriageReturns(sent_buffer);
                // istringstream istream(str);
                // istream.getline(buffer, sizeof(buffer), '\r');
                // std::cout << emailRetrieval.getMailContent() << std::endl;
                break;
            }
            Sleep(1000);
        }

        // std::cout << "Enter message (or 'exit' to quit): ";
        // std::cin.getline(buffer, sizeof(buffer));
        send(clientSocket, sent_buffer, sizeof(sent_buffer), 0);

        if (strcmp(sent_buffer, "exit") == 0) {
            break;
        }

        std::string response;
        char received_buffer[4096];
        int received_bytes;
        do {
            received_bytes = recv(clientSocket, received_buffer, sizeof(received_buffer), 0);
            if (received_bytes > 0) {
                response.append(received_buffer, received_bytes);
            } else if (received_bytes == 0) {
                std::cout << "Server closed the connection" << std::endl;
                break;
            } else {
                std::cerr << "recv failed with error: " << WSAGetLastError() << std::endl;
                break;
            }
        } while (received_bytes == sizeof(received_buffer));

        if (!response.empty()) {
            std::cout << "Server response: " << std::endl << response << std::endl;
        }
    }
    closesocket(clientSocket);
    WSACleanup();
    return 1;
}
