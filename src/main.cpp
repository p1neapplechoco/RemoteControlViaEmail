#include "./utils/Client.h"
#include "./utils/Server.h"

#include "./utils/GetWinDirectory.h"

int main()
{
    Client client;
    client.startClient();

    Server server;

    return 0;
}
