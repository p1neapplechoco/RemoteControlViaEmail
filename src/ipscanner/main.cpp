#include <iostream>
#include <string>
#include <WinSock2.h>
#include <WS2tcpip.h>
#pragma comment(lib, "ws2_32.lib")

class NetworkDiscovery {
private:
    SOCKET broadcastSocket;
    const int PORT = 45678;  // Choose an available port
    const std::string DISCOVERY_MESSAGE = "DISCOVER_REQUEST";
    
public:
    NetworkDiscovery() {
        // Initialize Winsock
        WSADATA wsaData;
        if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
            throw std::runtime_error("WSAStartup failed");
        }
        
        // Create UDP socket
        broadcastSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
        if (broadcastSocket == INVALID_SOCKET) {
            WSACleanup();
            throw std::runtime_error("Socket creation failed");
        }
        
        // Enable broadcasting
        BOOL broadcastEnable = TRUE;
        if (setsockopt(broadcastSocket, SOL_SOCKET, SO_BROADCAST, 
                      (char*)&broadcastEnable, sizeof(broadcastEnable)) < 0) {
            closesocket(broadcastSocket);
            WSACleanup();
            throw std::runtime_error("Failed to enable broadcasting");
        }
    }
    
    ~NetworkDiscovery() {
        closesocket(broadcastSocket);
        WSACleanup();
    }

    SOCKET getSocket() const {
        return broadcastSocket;
    }

    void sendBroadcast() {
        sockaddr_in broadcastAddr;
        broadcastAddr.sin_family = AF_INET;
        broadcastAddr.sin_port = htons(PORT);
        broadcastAddr.sin_addr.s_addr = INADDR_BROADCAST;
        
        if (sendto(broadcastSocket, DISCOVERY_MESSAGE.c_str(), 
                  DISCOVERY_MESSAGE.length(), 0,
                  (sockaddr*)&broadcastAddr, sizeof(broadcastAddr)) < 0) {
            throw std::runtime_error("Failed to send broadcast");
        }
    }
    
    void listenForResponses(int timeoutSeconds = 5) {
        // Set receive timeout
        DWORD timeout = timeoutSeconds * 1000;
        if (setsockopt(broadcastSocket, SOL_SOCKET, SO_RCVTIMEO, 
                      (char*)&timeout, sizeof(timeout)) < 0) {
            throw std::runtime_error("Failed to set timeout");
        }
        
        char buffer[1024];
        sockaddr_in senderAddr;
        int senderAddrLen = sizeof(senderAddr);
        
        std::cout << "Listening for responses...\n";
        
        while (true) {
            int bytesReceived = recvfrom(broadcastSocket, buffer, sizeof(buffer), 0,
                                       (sockaddr*)&senderAddr, &senderAddrLen);
                                       
            if (bytesReceived < 0) {
                if (WSAGetLastError() == WSAETIMEDOUT) {
                    std::cout << "Discovery complete\n";
                    break;
                }
                throw std::runtime_error("Error receiving response");
            }
            
            buffer[bytesReceived] = '\0';
            char senderIP[INET_ADDRSTRLEN];
            inet_ntop(AF_INET, &(senderAddr.sin_addr), senderIP, INET_ADDRSTRLEN);
            
            std::cout << "Response from " << senderIP << ": " << buffer << "\n";
        }
    }
};

// Example response server
class DiscoveryResponder {
private:
    SOCKET serverSocket;
    const int PORT = 45678;
    const std::string RESPONSE_MESSAGE = "DISCOVERY_RESPONSE";
    
public:
    DiscoveryResponder() {
        // Initialize Winsock
        WSADATA wsaData;
        if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
            throw std::runtime_error("WSAStartup failed");
        }
        
        // Create UDP socket
        serverSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
        if (serverSocket == INVALID_SOCKET) {
            WSACleanup();
            throw std::runtime_error("Socket creation failed");
        }
        
        // Bind socket
        sockaddr_in serverAddr;
        serverAddr.sin_family = AF_INET;
        serverAddr.sin_port = htons(PORT);
        serverAddr.sin_addr.s_addr = INADDR_ANY;
        
        if (bind(serverSocket, (sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
            closesocket(serverSocket);
            WSACleanup();
            throw std::runtime_error("Bind failed");
        }
    }
    
    ~DiscoveryResponder() {
        closesocket(serverSocket);
        WSACleanup();
    }
    
    void listen() {
        char buffer[1024];
        sockaddr_in senderAddr;
        int senderAddrLen = sizeof(senderAddr);
        
        std::cout << "Listening for discovery requests...\n";
        
        while (true) {
            int bytesReceived = recvfrom(serverSocket, buffer, sizeof(buffer), 0,
                                       (sockaddr*)&senderAddr, &senderAddrLen);
                                       
            if (bytesReceived < 0) {
                throw std::runtime_error("Error receiving request");
            }
            
            buffer[bytesReceived] = '\0';
            if (std::string(buffer) == "DISCOVER_REQUEST") {
                // Send response
                if (sendto(serverSocket, RESPONSE_MESSAGE.c_str(), 
                          RESPONSE_MESSAGE.length(), 0,
                          (sockaddr*)&senderAddr, senderAddrLen) < 0) {
                    throw std::runtime_error("Failed to send response");
                }
            }
        }
    }
};

// Example usage
int main() {
    try {
        // To discover other computers:
        NetworkDiscovery discovery;
        discovery.sendBroadcast();
        discovery.listenForResponses();
        
        // Or to respond to discovery requests:
        // DiscoveryResponder responder;
        // responder.listen();
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}