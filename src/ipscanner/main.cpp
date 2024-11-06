#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iphlpapi.h>

#pragma comment(lib, "iphlpapi.lib")
#pragma comment(lib, "ws2_32.lib")

// ICMP header structure
struct ICMPHeader {
    BYTE type;
    BYTE code;
    USHORT checksum;
    USHORT id;
    USHORT sequence;
};

// Calculate the ICMP checksum
USHORT calculateChecksum(USHORT* buffer, int size) {
    unsigned long cksum = 0;
    while (size > 1) {
        cksum += *buffer++;
        size -= sizeof(USHORT);
    }
    if (size) {
        cksum += *(UCHAR*)buffer;
    }
    cksum = (cksum >> 16) + (cksum & 0xffff);
    cksum += (cksum >> 16);
    return (USHORT)(~cksum);
}

// Send ICMP echo request and wait for reply
bool sendICMPRequest(const std::string& ipAddress) {
    SOCKET sock = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
    if (sock == INVALID_SOCKET) {
        return false;
    }

    sockaddr_in dest;
    dest.sin_family = AF_INET;
    inet_pton(AF_INET, ipAddress.c_str(), &(dest.sin_addr));

    const int ICMP_ECHO = 8;
    const int ICMP_ECHOREPLY = 0;
    const int PACKET_SIZE = sizeof(ICMPHeader) + 32;

    char sendBuf[PACKET_SIZE] = {0};
    ICMPHeader* icmpHeader = (ICMPHeader*)sendBuf;
    icmpHeader->type = ICMP_ECHO;
    icmpHeader->code = 0;
    icmpHeader->id = (USHORT)GetCurrentProcessId();
    icmpHeader->sequence = 0;
    icmpHeader->checksum = calculateChecksum((USHORT*)sendBuf, PACKET_SIZE);

    if (sendto(sock, sendBuf, PACKET_SIZE, 0, (sockaddr*)&dest, sizeof(dest)) == SOCKET_ERROR) {
        closesocket(sock);
        return false;
    }

    char recvBuf[PACKET_SIZE];
    int bytesReceived = recv(sock, recvBuf, PACKET_SIZE, 0);
    closesocket(sock);

    if (bytesReceived > 0) {
        ICMPHeader* receivedHeader = (ICMPHeader*)(recvBuf + 20); // Skip IP header
        return (receivedHeader->type == ICMP_ECHOREPLY);
    }

    return false;
}

std::vector<std::string> scanNetwork(const std::string& ipAddress, const std::string& subnetMask) {
    std::vector<std::string> activeHosts;

    unsigned long ipAddr = inet_addr(ipAddress.c_str());
    unsigned long subnetMaskAddr = inet_addr(subnetMask.c_str());
    unsigned long networkAddr = ipAddr & subnetMaskAddr;
    unsigned long broadcastAddr = networkAddr | ~subnetMaskAddr;

    for (unsigned long i = networkAddr + 1; i < broadcastAddr; i++) {
        in_addr addr;
        addr.s_addr = i;
        std::string currentIp = inet_ntoa(addr);

        if (sendICMPRequest(currentIp)) {
            activeHosts.push_back(currentIp);
        }
    }

    return activeHosts;
}

int main() {
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "WSAStartup failed.\n";
        return 1;
    }

    std::string ipAddress = "26.81.48.125";  // Your IP address
    std::string subnetMask = "255.0.0.0";    // Your subnet mask

    std::vector<std::string> activeHosts = scanNetwork(ipAddress, subnetMask);

    std::cout << "Active hosts on the network:" << std::endl;
    for (const auto& host : activeHosts) {
        std::cout << host << std::endl;
    }

    WSACleanup();
    return 0;
}