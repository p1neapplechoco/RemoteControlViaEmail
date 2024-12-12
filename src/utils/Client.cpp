#include <iostream>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <string>
#include <vector>
#include <fstream>
#include <stdexcept>
#include "networkDiscovery.h"
#include "Client.h"
#include <chrono>
#pragma comment(lib, "ws2_32.lib")

void removeCarriageReturns(char *str)
{
    char *write = str;

    for (const char *read = str; *read; read++)
        if (*read != '\r')
            *write++ = *read;

    *write = '\0';
}

Client::Client() = default;

Client::~Client()
{
    WSACleanup();
    closesocket(clientSocket);
}

bool Client::setupClient()
{
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "Failed to initialize Winsock" << std::endl;
        return false;
    }

    clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket == INVALID_SOCKET) {
        std::cerr << "Failed to create socket" << std::endl;
        WSACleanup();
        return false;
    }
    return true;
}

bool Client::connectToServer(string serverIP)
{
    if(!setupClient())
        return false;

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(PORT);
    inet_pton(AF_INET, serverIP.c_str(), &serverAddr.sin_addr);

    if (connect(clientSocket, (sockaddr *) &serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        std::cerr << "Connection failed" << std::endl;
        closesocket(clientSocket);
        WSACleanup();
        return false;
    }
    return true;
}

vector<string> Client::scanIP()
{
    NetworkDiscovery network_discovery;
    network_discovery.sendBroadcast();
    network_discovery.listenForResponses(5);
    return network_discovery.getDiscoveredIPs();
}

std::vector<char> Client::receiveImageData() {
    std::vector<char> buffer;
    int bytesReceived;
    int totalBytesReceived = 0;
    int expectedSize = 0;

    bytesReceived = recv(clientSocket, (char *) &expectedSize, sizeof(int), 0);
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

bool Client::handleCommand(const string &command, string &reponseClient, string &filePath) {
    char sendBuffer[1024] = {};
    if (command.length() < sizeof(sendBuffer)) {
        command.copy(sendBuffer, sizeof(sendBuffer) - 1);
        sendBuffer[command.length()] = '\0';
    } else {
        std::cerr << "Error: Command too long for buffer." << std::endl;
    }

    // List RadminVPN devices
    if (std::string(sendBuffer) == "list network") {
        scanIP();
        return true;
    }

    send(clientSocket, sendBuffer, strlen(sendBuffer), 0);

    if (strcmp(sendBuffer, "!exit") == 0) {
        reponseClient = "Disconnected with Server";
        return true;
    }

    // Receive attachments from server
    // Receive response from server
    std::vector<char> receivedData;
    int bytesReceived;
    int expectedSize = 0;

    // Handle image data for both screen capture and webcam
    if (strcmp(sendBuffer, "!screenshot") == 0 || strcmp(sendBuffer, "!capture") == 0) {
        std::vector<char> imageData = receiveImageData();
        if (!imageData.empty()) {
            std::string filename;
            if (strcmp(sendBuffer, "screenshot") == 0) {
                filename = filePath = "screenshot.jpg";
            } else {
                // Create filename with timestamp for webcam frames
                auto now = std::chrono::system_clock::now();
                auto now_c = std::chrono::system_clock::to_time_t(now);
                char timestamp[20];
                strftime(timestamp, sizeof(timestamp), "%Y%m%d_%H%M%S", localtime(&now_c));
                filename = filePath = "webcam_" + std::string(timestamp) + ".jpg";
            }

            std::ofstream outFile(filename, std::ios::binary);
            outFile.write(imageData.data(), imageData.size());
            outFile.close();
            std::cout << "Image saved as " << filename << std::endl;
            reponseClient = "Image saved as " + filename + "\n";
        }
    }

    if (strstr(sendBuffer, "!get file") != NULL) {
        // Receive file size
        int fileSize;
        if (recv(clientSocket, reinterpret_cast<char *>(&fileSize), sizeof(int), 0) <= 0) {
            std::cerr << "Failed to receive file size" << std::endl;
            return false;
        }

        // Receive file data
        std::vector<char> fileBuffer(fileSize);
        int totalReceived = 0;
        while (totalReceived < fileSize) {
            int bytesReceived = recv(clientSocket, fileBuffer.data() + totalReceived,
                                         fileSize - totalReceived, 0);
            if (bytesReceived <= 0) {
                std::cerr << "Failed to receive file data" << std::endl;
                return false;
            }
            totalReceived += bytesReceived;
        }

        // Write to file
        std::string tmp(sendBuffer);
        std::string outputPath = tmp.substr(tmp.find_last_of("\\") + 1);
        std::ofstream outFile(outputPath, std::ios::binary);
        if (!outFile) {
            std::cerr << "Failed to create output file" << std::endl;
            return false;
        }

        outFile.write(fileBuffer.data(), fileSize);
        outFile.close();

        std::cout << "File received and saved to: " << outputPath << std::endl;
        reponseClient = "File received and saved to: " + outputPath + "\n";
    }

    if (strstr(sendBuffer, "!index") != NULL) {
        std::string str(sendBuffer + 7);

        int numDisks = 0;
        if (str.empty()) {
            recv(clientSocket, (char *) &numDisks, sizeof(int), 0);
            for (int i = 0; i < numDisks; i++) {
                // Get disk name
                char diskName[3];
                recv(clientSocket, diskName, sizeof(diskName), 0);

                // Receive file size
                int fileSize;
                if (recv(clientSocket, reinterpret_cast<char *>(&fileSize), sizeof(int), 0) <= 0) {
                    std::cerr << "Failed to receive file size" << std::endl;
                    return false;
                }

                // Receive file data
                std::vector<char> fileBuffer(fileSize);
                int totalReceived = 0;
                while (totalReceived < fileSize) {
                    int bytesReceived = recv(clientSocket, fileBuffer.data() + totalReceived,
                                                 fileSize - totalReceived, 0);
                    if (bytesReceived <= 0) {
                        std::cerr << "Failed to receive file data" << std::endl;
                        return false;
                    }
                    totalReceived += bytesReceived;
                }

                // Write to file
                std::string tmp(diskName);
                tmp = tmp.substr(0, 1);
                std::ofstream outFile("cache_" + tmp + ".txt", std::ios::binary);
                outFile.write(fileBuffer.data(), fileSize);
                outFile.close();
            }
        } else {
            // Receive file size
            int fileSize;
            if (recv(clientSocket, reinterpret_cast<char *>(&fileSize), sizeof(int), 0) <= 0) {
                std::cerr << "Failed to receive file size" << std::endl;
                return false;
            }
            // Receive file data
            std::vector<char> fileBuffer(fileSize);
            int totalReceived = 0;
            while (totalReceived < fileSize) {
                int bytesReceived = recv(clientSocket, fileBuffer.data() + totalReceived,
                                             fileSize - totalReceived, 0);
                if (bytesReceived <= 0) {
                    std::cerr << "Failed to receive file data" << std::endl;
                    return false;
                }
                totalReceived += bytesReceived;
            }

            // Write to file
            str = str.substr(0, 1);
            std::ofstream outFile("cache_" + str + ".txt", std::ios::binary);
            outFile.write(fileBuffer.data(), fileSize);
            outFile.close();
        }
        // TODO: Implement indexing
    }

    // If webcam is started, you can automatically request frames
    if (strcmp(sendBuffer, "start webcam") == 0) {
        std::cout << "Webcam started. Use 'get webcam frame' to capture frames.\n";
        reponseClient = "Webcam started. Use 'get webcam frame' to capture frames.\n";
    }

    // Receive response msg from server
    // First, receive the size of the response
    bytesReceived = recv(clientSocket, (char *) &expectedSize, sizeof(int), 0);
    if (bytesReceived != sizeof(int)) {
        std::cerr << "Failed to receive response size" << std::endl;
        return false;
    }

    char recvBuffer[expectedSize];
    // Now receive the actual response
    while (receivedData.size() < expectedSize) {
        bytesReceived = recv(clientSocket, recvBuffer, expectedSize, 0);
        if (bytesReceived > 0) {
            receivedData.insert(receivedData.end(), recvBuffer, recvBuffer + bytesReceived);
        } else if (bytesReceived == 0) {
            std::cout << "Server closed the connection" << std::endl;
            reponseClient += "Server closed the connection\n";
            break;
        } else {
            std::cerr << "recv failed with error: " << WSAGetLastError() << std::endl;
            break;
        }
    }

    if (!receivedData.empty()) {
        std::string response(receivedData.begin(), receivedData.end());
        std::cout << "Server response: " << std::endl << response << std::endl;
        reponseClient += response + "\n";
    }
    return true;
}

/*
bool Client::startClient() {
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "Failed to initialize Winsock" << std::endl;
        return false;
    }

    SOCKET clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket == INVALID_SOCKET) {
        std::cerr << "Failed to create socket" << std::endl;
        WSACleanup();
        return false;
    }

    scanIP();

    std::string serverIP;

    std::cout << "Enter server IP address: ";
    std::cin >> serverIP;

    std::cin.ignore(); // Clear the newline from the input buffer
    if(!connectToServer(serverIP))
        return false;

    std::cout << "Connected to server at " << serverIP << ":" << PORT << std::endl;

    std::cout << "!help for list of commands" << std::endl;

    UserCredentials user;
    user.loadCredentials();
    email_retrieval = EmailRetrieval(user);
    email_retrieval.setupCurl();

    bool isReceiving = false;
    while (true) {
        char sendBuffer[1024] = {};

        if(!isReceiving) {
            std::cout << "Enter your message: ";
            std::cin.getline(sendBuffer, sizeof(sendBuffer));
        } else {
            std::string prev_mail_id = " ";
            bool fl = true;
            while (true)
            {
                email_retrieval.retrieveEmail();
                if (fl)
                {
                    prev_mail_id = email_retrieval.getMailID();
                    fl = false;
                    std::cout << "Ready to retrieve mails." << std::endl;
                }
                if (prev_mail_id != email_retrieval.getMailID())
                {
                    std::string str = email_retrieval.getMailContent();
                    strcpy(sendBuffer, email_retrieval.getMailContent().c_str());
                    removeCarriageReturns(sendBuffer);
                    break;
                }
            }
        }

        if (std::string(sendBuffer) == "receive email") {
            isReceiving = true;
            continue;
        }

        // List RadminVPN devices
        if (std::string(sendBuffer) == "list network") {
            scanIP();
            continue;
        }

        send(clientSocket, sendBuffer, strlen(sendBuffer), 0);

        if (strcmp(sendBuffer, "!exit") == 0) {
            break;
        }

        // Receive attachments from server
        // Receive response from server
        std::vector<char> receivedData;
        int bytesReceived;
        int expectedSize = 0;

        // Handle image data for both screen capture and webcam
        if (strcmp(sendBuffer, "!screenshot") == 0 || strcmp(sendBuffer, "!capture") == 0) {
            std::vector<char> imageData = receiveImageData();
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

        if (strstr(sendBuffer, "!get file") != NULL) {
            // Receive file size
            int fileSize;
            if (recv(clientSocket, reinterpret_cast<char *>(&fileSize), sizeof(int), 0) <= 0) {
                std::cerr << "Failed to receive file size" << std::endl;
                return false;
            }

            // Receive file data
            std::vector<char> fileBuffer(fileSize);
            int totalReceived = 0;
            while (totalReceived < fileSize) {
                int bytesReceived = recv(clientSocket, fileBuffer.data() + totalReceived,
                                         fileSize - totalReceived, 0);
                if (bytesReceived <= 0) {
                    std::cerr << "Failed to receive file data" << std::endl;
                    return false;
                }
                totalReceived += bytesReceived;
            }

            // Write to file
            std::string tmp(sendBuffer);
            std::string outputPath = tmp.substr(tmp.find_last_of("\\") + 1);
            std::ofstream outFile(outputPath, std::ios::binary);
            if (!outFile) {
                std::cerr << "Failed to create output file" << std::endl;
                return false;
            }

            outFile.write(fileBuffer.data(), fileSize);
            outFile.close();

            std::cout << "File received and saved to: " << outputPath << std::endl;
        }

        if (strstr(sendBuffer, "!index") != NULL) {
            std::string str(sendBuffer + 7);

            int numDisks = 0;
            if (str.empty()) {
                recv(clientSocket, (char *) &numDisks, sizeof(int), 0);
                for (int i = 0; i < numDisks; i++) {
                    // Get disk name
                    char diskName[3];
                    recv(clientSocket, diskName, sizeof(diskName), 0);

                    // Receive file size
                    int fileSize;
                    if (recv(clientSocket, reinterpret_cast<char *>(&fileSize), sizeof(int), 0) <= 0) {
                        std::cerr << "Failed to receive file size" << std::endl;
                        return false;
                    }

                    // Receive file data
                    std::vector<char> fileBuffer(fileSize);
                    int totalReceived = 0;
                    while (totalReceived < fileSize) {
                        int bytesReceived = recv(clientSocket, fileBuffer.data() + totalReceived,
                                                 fileSize - totalReceived, 0);
                        if (bytesReceived <= 0) {
                            std::cerr << "Failed to receive file data" << std::endl;
                            return false;
                        }
                        totalReceived += bytesReceived;
                    }

                    // Write to file
                    std::string tmp(diskName);
                    tmp = tmp.substr(0, 1);
                    std::ofstream outFile("cache_" + tmp + ".txt", std::ios::binary);
                    outFile.write(fileBuffer.data(), fileSize);
                    outFile.close();
                }
            } else {
                // Receive file size
                int fileSize;
                if (recv(clientSocket, reinterpret_cast<char *>(&fileSize), sizeof(int), 0) <= 0) {
                    std::cerr << "Failed to receive file size" << std::endl;
                    return false;
                }
                // Receive file data
                std::vector<char> fileBuffer(fileSize);
                int totalReceived = 0;
                while (totalReceived < fileSize) {
                    int bytesReceived = recv(clientSocket, fileBuffer.data() + totalReceived,
                                             fileSize - totalReceived, 0);
                    if (bytesReceived <= 0) {
                        std::cerr << "Failed to receive file data" << std::endl;
                        return false;
                    }
                    totalReceived += bytesReceived;
                }

                // Write to file
                str = str.substr(0, 1);
                std::ofstream outFile("cache_" + str + ".txt", std::ios::binary);
                outFile.write(fileBuffer.data(), fileSize);
                outFile.close();
            }
            // TODO: Implement indexing
        }

        // If webcam is started, you can automatically request frames
        if (strcmp(sendBuffer, "start webcam") == 0) {
            std::cout << "Webcam started. Use 'get webcam frame' to capture frames.\n";
        }

        // Receive response msg from server
        // First, receive the size of the response
        bytesReceived = recv(clientSocket, (char *) &expectedSize, sizeof(int), 0);
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
    }

    closesocket(clientSocket);
    WSACleanup();
    return true;
}
*/
