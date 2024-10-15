#include <iostream>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <string>
#include <vector>
#include <fstream>

#pragma comment(lib, "ws2_32.lib")

std::vector<char> receiveImageData(SOCKET clientSocket) {
    std::vector<char> buffer;
    char chunk[4096];
    int bytesReceived;
    do {
        bytesReceived = recv(clientSocket, chunk, sizeof(chunk), 0);
        if (bytesReceived > 0) {
            buffer.insert(buffer.end(), chunk, chunk + bytesReceived);
        }
    } while (bytesReceived == sizeof(chunk));
    return buffer;
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
    std::cin.ignore();  // Clear the newline from the input buffer

    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(serverPort);
    inet_pton(AF_INET, serverIP.c_str(), &serverAddr.sin_addr);

    if (connect(clientSocket, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        std::cerr << "Connection failed" << std::endl;
        closesocket(clientSocket);
        WSACleanup();
        return 1;
    }

    std::cout << "Connected to server at " << serverIP << ":" << serverPort << std::endl;

    while (true) {
        char sendBuffer[1024] = {};
        std::cout << "Enter message (or 'exit' to quit): ";
        std::cin.getline(sendBuffer, sizeof(sendBuffer));
        send(clientSocket, sendBuffer, strlen(sendBuffer), 0);

        if (strcmp(sendBuffer, "exit") == 0) {
            break;
        }

        // Receive response from server
        std::string receivedData;
        char recvBuffer[4096];
        int bytesReceived;
        do {
            bytesReceived = recv(clientSocket, recvBuffer, sizeof(recvBuffer), 0);
            if (bytesReceived > 0) {
                receivedData.append(recvBuffer, bytesReceived);
            } else if (bytesReceived == 0) {
                std::cout << "Server closed the connection" << std::endl;
                break;
            } else {
                std::cerr << "recv failed with error: " << WSAGetLastError() << std::endl;
                break;
            }
        } while (bytesReceived == sizeof(recvBuffer));

        if (!receivedData.empty()) {
            std::cout << "Server response: " << std::endl << receivedData << std::endl;
        }
        if (strcmp(sendBuffer, "screen capture") == 0) {
            std::vector<char> imageData = receiveImageData(clientSocket);
            std::ofstream outFile("screenshot.jpg", std::ios::binary);
            outFile.write(imageData.data(), imageData.size());
            outFile.close();
            std::cout << "Screenshot saved as screenshot.jpg" << std::endl;
        }
    }

    closesocket(clientSocket);
    WSACleanup();
    return 0;
}