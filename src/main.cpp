// #include "utils/Client.h"
#include "utils/Server.h"

#pragma comment(lib, "ws2_32.lib")

int main()
{
    // Client client;
    // client.startClient();

    Server server;
    server.startServer();
    return 0;
}
