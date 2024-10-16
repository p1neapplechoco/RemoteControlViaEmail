#include "EmailRetrieval.h"
#include <vector>
#include <iostream>
#include <winsock2.h>
#include <stdio.h>
#pragma comment(lib, "ws2_32.lib")

using namespace std;
// Place this file in root/src

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

    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1"); // parametrize this
    serverAddr.sin_port = htons(serverPort); // parametrize this

    // Connect to the server
    if (connect(clientSocket, (sockaddr *) &serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
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
        char buffer[1024] = {};
        std::string prev_mail_id = " ";
        bool fl = true;
        while (true) {
            emailRetrieval.retrieveEmail();
            if (fl) {
                prev_mail_id = emailRetrieval.getMailID();
                fl = false;
            }
            if (prev_mail_id != emailRetrieval.getMailID()) {
                string str = emailRetrieval.getMailContent();
                strcpy(buffer, emailRetrieval.getMailContent().c_str());
                removeCarriageReturns(buffer);
                // istringstream istream(str);
                // istream.getline(buffer, sizeof(buffer), '\r');
                std::cout << emailRetrieval.getMailContent().length() << std::endl;
                break;
            }
            Sleep(1000);
        }

        // std::cout << "Enter message (or 'exit' to quit): ";
        // std::cin.getline(buffer, sizeof(buffer));
        cout << buffer << "CC" << endl;
        send(clientSocket, buffer, sizeof(buffer), 0);

        if (strcmp(buffer, "exit") == 0) {
            break;
        }

        // Receive response from server
        memset(buffer, 0, sizeof(buffer));
        int bytesReceived = recv(clientSocket, buffer, sizeof(buffer), 0);
        if (bytesReceived > 0) {
            // printf("Server response: %s\n", buffer);
            std::cout << "Server response: ";
            std::cout << string(buffer) << std::endl;
        } else {
            std::cerr << "Failed to receive response from server" << std::endl;
        }
    }
    // Clean-up
    closesocket(clientSocket);
    WSACleanup();
    return 1;
}

//-------------------------------------------------------------------------------------------------------------------/
//-------------------------------------------------------------------------------------------------------------------/
//-------------------------------------------------------------------------------------------------------------------/


//     UserCredentials user;
//     user.loadCredentials();
//     EmailRetrieval emailRetrieval(user);
//
//     emailRetrieval.setupCurl();
//
//     std::string prev_mail_id = " ";
//     while (true)
//     {
//         emailRetrieval.retrieveEmail();
//         if (prev_mail_id != emailRetrieval.getMailID())
//         {
//             std::cout << emailRetrieval.getMailContent() << std::endl;
//             prev_mail_id = emailRetrieval.getMailID();
//         }
//         Sleep(1000);
//     }
// //    for (std::string mail : contents_of_mails)
// //    {
// //        std::cout << mail << std::endl;
// //    }
//     return 1;
// }
