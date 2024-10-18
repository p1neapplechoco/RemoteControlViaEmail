#include <iostream>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <string>
#include <vector>
#include <fstream>

#pragma comment(lib, "ws2_32.lib")

std::vector<char> receiveImageData(SOCKET clientSocket) {
    std::vector<char> buffer;
    int bytesReceived;
    int totalBytesReceived = 0;
    int expectedSize = 0;

    // First, receive the size of the image
    bytesReceived = recv(clientSocket, (char*)&expectedSize, sizeof(int), 0);
    if (bytesReceived != sizeof(int)) {
        std::cerr << "Failed to receive image size" << std::endl;
        return buffer;
    }
    std::cout << "Receiving image data (" << expectedSize << " bytes)" << std::endl;
    char chunk[expectedSize];
    // Now receive the image data
    while (totalBytesReceived < expectedSize) {
        bytesReceived = recv(clientSocket, chunk, expectedSize, 0);
        if (bytesReceived > 0) {
            buffer.insert(buffer.end(), chunk, chunk + bytesReceived);
            totalBytesReceived += bytesReceived;
        } else if (bytesReceived == 0) {
            std::cout << "Connection closed by server" << std::endl;
            break;
        } else {
            std::cerr << "recv failed with error: " << WSAGetLastError() << std::endl;
            break;
        }
    }

    if (totalBytesReceived != expectedSize) {
        std::cerr << "Received " << totalBytesReceived << " bytes, expected " << expectedSize << " bytes" << std::endl;
    }

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
        std::vector<char> receivedData;

        int bytesReceived;
        int expectedSize = 0;

        // First, receive the size of the response
        bytesReceived = recv(clientSocket, (char*)&expectedSize, sizeof(int), 0);
        if (bytesReceived != sizeof(int)) {
            std::cerr << "Failed to receive response size" << std::endl;
            continue;
        }
        char recvBuffer[expectedSize];
        // Now receive the actual response
        while (receivedData.size() < expectedSize) {

            bytesReceived = recv(clientSocket, recvBuffer, expectedSize, 0);
            if (bytesReceived > 0) {
                receivedData.insert(receivedData.end(), recvBuffer, recvBuffer + bytesReceived);
            } else if (bytesReceived == 0) {
                std::cout << "Server closed the connection" << std::endl;
                break;
            } else {
                std::cerr << "recv failed with error: " << WSAGetLastError() << std::endl;
                break;
            }
        }

        if (!receivedData.empty()) {
            std::string response(receivedData.begin(), receivedData.end());
            std::cout << "Server response: " << std::endl << response << std::endl;
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