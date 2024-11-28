#include "Client.h"
#include <fstream>
#include <iostream>
#include <ws2tcpip.h>
#include "IpDiscovery.h"

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
    closesocket(client_socket);
}

bool Client::setupWSA()
{
    return WSAStartup(MAKEWORD(2, 2), &wsa_data) == 0;
}

bool Client::setupSocket()
{
    client_socket = socket(AF_INET, SOCK_STREAM, 0);
    return client_socket != INVALID_SOCKET;
}

bool Client::setupClient()
{
    if (!setupWSA())
    {
        std::cerr << "Failed to setup WSA" << std::endl;
        return false;
    }
    if (!setupSocket())
    {
        std::cerr << "Failed to setup socket" << std::endl;
        WSACleanup();
        return false;
    }

    scanIP();

    std::cout << "Enter server IP: ";
    std::cin >> server_ip;
    std::cout << "Enter server port: ";
    std::cin >> server_port;

    return connectToServer();
}

bool Client::connectToServer()
{
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(server_port);
    inet_pton(AF_INET, server_ip.c_str(), &server_address.sin_addr);

    if (connect(client_socket, reinterpret_cast<sockaddr *>(&server_address), sizeof(server_address)) == SOCKET_ERROR)
    {
        std::cerr << "Connection failed" << std::endl;
        closesocket(client_socket);
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

std::vector<char> Client::receiveImageData() const
{
    std::vector<char> buffer;
    int total_bytes_received = 0;
    int expected_size = 0;

    int received_bytes = recv(client_socket, reinterpret_cast<char *>(&expected_size), sizeof(int), 0);
    if (received_bytes != sizeof(int))
    {
        std::cerr << "Failed to receive image size" << std::endl;
        return buffer;
    }
    std::cout << "Receiving image data (" << expected_size << " bytes)" << std::endl;

    while (total_bytes_received < expected_size)
    {
        char chunk[expected_size];
        received_bytes = recv(client_socket, chunk, expected_size, 0);
        if (received_bytes > 0)
        {
            buffer.insert(buffer.end(), chunk, chunk + received_bytes);
            total_bytes_received += received_bytes;
        } else if (received_bytes == 0)
        {
            std::cout << "Connection closed by server" << std::endl;
            break;
        } else
        {
            std::cerr << "recv failed with error: " << WSAGetLastError() << std::endl;
            break;
        }
    }

    if (total_bytes_received != expected_size)
        std::cerr << "Received " << total_bytes_received << " bytes, expected " << expected_size << " bytes" << std::endl;

    return buffer;
}

void Client::startClient()
{
    if (!setupClient())
    {
        std::cerr << "Failed to setup client" << std::endl;
        closesocket(client_socket);
        WSACleanup();
        return;
    }

    UserCredentials user;
    user.loadCredentials();
    email_retrieval = EmailRetrieval(user);
    email_retrieval.setupCurl();

    while (true)
    {
        char sent_buffer[1024] = {};
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
                strcpy(sent_buffer, email_retrieval.getMailContent().c_str());
                removeCarriageReturns(sent_buffer);
                break;
            }
            // Sleep(1000);
        }

        send(client_socket, sent_buffer, sizeof(sent_buffer), 0);

        if (strcmp(sent_buffer, "exit") == 0)
        {
            break;
        }

        std::vector<char> received_data;

        int received_bytes;
        int expected_size = 0;

        received_bytes = recv(client_socket, reinterpret_cast<char *>(&expected_size), sizeof(int), 0);
        if (received_bytes != sizeof(int))
        {
            std::cerr << "Failed to receive response size" << std::endl;
            continue;
        }

        while (received_data.size() < expected_size)
        {
            char received_buffer[expected_size];
            received_bytes = recv(client_socket, received_buffer, expected_size, 0);
            if (received_bytes > 0)
                received_data.insert(received_data.end(), received_buffer, received_buffer + received_bytes);
            else if (received_bytes == 0)
            {
                std::cout << "Server closed the connection" << std::endl;
                break;
            } else
            {
                std::cerr << "recv failed with error: " << WSAGetLastError() << std::endl;
                break;
            }
        }

        if (!received_data.empty())
        {
            std::string response(received_data.begin(), received_data.end());
            std::cout << "Server response: " << std::endl
                    << response << std::endl;
        }

        if (strcmp(sent_buffer, "screen capture") == 0)
        {
            std::vector<char> image_data = receiveImageData();
            std::ofstream outFile("screenshot.jpg", std::ios::binary);
            outFile.write(image_data.data(), image_data.size());
            outFile.close();
            std::cout << "Screenshot saved as screenshot.jpg" << std::endl;
        }
    }
    closesocket(client_socket);
    WSACleanup();
}

bool Client::handleCommand(const string& command, string& reponseClient) {
    char sent_buffer[1024] = {};
    strncpy(sent_buffer, command.c_str(), sizeof(sent_buffer) - 1);

    // Send command to server
    send(client_socket, sent_buffer, sizeof(sent_buffer), 0);

    if (command == "exit") {
        closesocket(client_socket);
        WSACleanup();
        return false;
    }

    // Receive text response
    std::vector<char> received_data;
    int expected_size = 0;

    int received_bytes = recv(client_socket, reinterpret_cast<char*>(&expected_size), sizeof(int), 0);
    if (received_bytes != sizeof(int)) {
        std::cerr << "Failed to receive response size" << std::endl;
        closesocket(client_socket);
        WSACleanup();
        return false;
    }

    while (received_data.size() < expected_size) {
        char received_buffer[1024];
        received_bytes = recv(client_socket, received_buffer, sizeof(received_buffer), 0);
        if (received_bytes > 0) {
            received_data.insert(received_data.end(), received_buffer, received_buffer + received_bytes);
        } else if (received_bytes == 0) {
            std::cout << "Server closed the connection" << std::endl;
            break;
        } else {
            std::cerr << "recv failed with error: " << WSAGetLastError() << std::endl;
            break;
        }
    }

    if (!received_data.empty()) {
        std::string response(received_data.begin(), received_data.end());
        std::cout << "Server response:\n" << response << std::endl;
        reponseClient = response + '\n';
    }

    // Handle image data for screenshot and webcam capture commands
    if (command == "!screenshot" || command == "!capture") {
        std::vector<char> image_data = receiveImageData();
        if (!image_data.empty()) {
            std::string filename = (command == "!screenshot") ? "screenshot.png" : "webcam.png";
            std::remove(filename.c_str());

            std::ofstream outFile("./assert/capture/" + filename, std::ios::binary);
            outFile.write(image_data.data(), image_data.size());
            outFile.close();

            std::cout << "Image saved as " << filename << std::endl;
            reponseClient += "Image saved as " + filename + '\n';
        }
    }

    return true;
}
