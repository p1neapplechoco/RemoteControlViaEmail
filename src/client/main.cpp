#include <iostream>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <string>
#include <vector>
#include <fstream>
#include <stdexcept>
#include "networkDiscovery.h"
#include <chrono>
#pragma comment(lib, "ws2_32.lib")

std::vector<char> receiveImageData(SOCKET clientSocket) {
    std::vector<char> buffer;
    int bytesReceived;
    int totalBytesReceived = 0;
    int expectedSize = 0;

    bytesReceived = recv(clientSocket, (char*)&expectedSize, sizeof(int), 0);
    if (bytesReceived != sizeof(int)) {
        std::cerr << "Failed to receive image size" << std::endl;
        return buffer;
    }
    std::cout << "Receiving image data (" << expectedSize << " bytes)" << std::endl;
    char chunk[expectedSize];

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

    if (totalBytesReceived != expectedSize)
        std::cerr << "Received " << totalBytesReceived << " bytes, expected " << expectedSize << " bytes" << std::endl;

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

    // Scan IP first
    NetworkDiscovery initialDiscovery;
    initialDiscovery.sendBroadcast();
    initialDiscovery.listenForResponses(5);
    // Scan IP first

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

    std::cout << "!help for list of commands" << std::endl;

    while (true) {
        char sendBuffer[1024] = {};
        std::cout << "Enter your message: ";
        std::cin.getline(sendBuffer, sizeof(sendBuffer));

        // List RadminVPN devices
        if (std::string(sendBuffer) == "list network") {
            NetworkDiscovery networkDiscovery;
            networkDiscovery.sendBroadcast();
            networkDiscovery.listenForResponses(5);
            continue;
        }

        send(clientSocket, sendBuffer, strlen(sendBuffer), 0);

        if (strcmp(sendBuffer, "!exit") == 0) {
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

        // Handle image data for both screen capture and webcam
        if (strcmp(sendBuffer, "!screenshot") == 0 || strcmp(sendBuffer, "!capture") == 0) {
            std::vector<char> imageData = receiveImageData(clientSocket);
            if (!imageData.empty()) {
                std::string filename;
                if (strcmp(sendBuffer, "screen capture") == 0) {
                    filename = "screenshot.jpg";
                } else {
                    // Create filename with timestamp for webcam frames
                    auto now = std::chrono::system_clock::now();
                    auto now_c = std::chrono::system_clock::to_time_t(now);
                    char timestamp[20];
                    strftime(timestamp, sizeof(timestamp), "%Y%m%d_%H%M%S", localtime(&now_c));
                    filename = "webcam_" + std::string(timestamp) + ".jpg";
                }

                std::ofstream outFile(filename, std::ios::binary);
                outFile.write(imageData.data(), imageData.size());
                outFile.close();
                std::cout << "Image saved as " << filename << std::endl;
            }
        }

        // If webcam is started, you can automatically request frames
        if (strcmp(sendBuffer, "start webcam") == 0) {
            std::cout << "Webcam started. Use 'get webcam frame' to capture frames.\n";
        }
    }

    closesocket(clientSocket);
    WSACleanup();
    return 0;
}