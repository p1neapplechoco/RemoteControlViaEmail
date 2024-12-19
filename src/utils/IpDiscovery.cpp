#include "IpDiscovery.h"

DiscoveryResponder::DiscoveryResponder()
{
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
    {
        throw std::runtime_error("WSAStartup failed");
    }

    responder_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (responder_socket == INVALID_SOCKET)
    {
        WSACleanup();
        throw std::runtime_error("Socket creation failed");
    }

    sockaddr_in server_address{};
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(PORT);
    server_address.sin_addr.s_addr = INADDR_ANY;

    if (bind(responder_socket, reinterpret_cast<sockaddr *>(&server_address), sizeof(server_address)) < 0)
    {
        closesocket(responder_socket);
        WSACleanup();
        throw std::runtime_error("Bind failed");
    }
}

DiscoveryResponder::~DiscoveryResponder()
{
    closesocket(responder_socket);
    WSACleanup();
}

void DiscoveryResponder::listen() const
{
    char buffer[1024];
    sockaddr_in sender_address{};
    int sender_size = sizeof(sender_address);

    std::cout << "Listening for discovery requests...\n";

    while (true)
    {
        std::cout << "skibidi" << std::endl;
        const int received_bytes = recvfrom(responder_socket, buffer, sizeof(buffer), 0,
                                            reinterpret_cast<sockaddr *>(&sender_address), &sender_size); // Waiting

        std::cout << received_bytes << std::endl;
        if (received_bytes < 0)
        {
            throw std::runtime_error("Error receiving request");
        }
        buffer[received_bytes] = '\0';
        if (std::string(buffer) == "DISCOVER_REQUEST")
        {
            // Send response
            if (sendto(responder_socket, RESPONSE_MESSAGE.c_str(),
                       RESPONSE_MESSAGE.size(), 0,
                       reinterpret_cast<sockaddr *>(&sender_address), sender_size) < 0)
            {
                throw std::runtime_error("Failed to send response");
            }
            break;
        }
    }
}
