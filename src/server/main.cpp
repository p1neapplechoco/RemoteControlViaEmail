//
// Created by phida on 10/9/2024.
//

#include "main.h"


using namespace std;

void handleClient(SOCKET clientSocket) {
    char buffer[1024];
    while (true) {
        memset(buffer, 0, sizeof(buffer));
        int bytesReceived = recv(clientSocket, buffer, sizeof(buffer), 0);
        if (bytesReceived <= 0) {
            cout << "Client disconnected." << endl;
            break;
        }

        cout << "Received: " << buffer << endl;

        if (strcmp(buffer, "exit") == 0) {
            cout << "Client requested to exit." << endl;
            break;
        }

        string response = string(buffer);
        transform(response.begin(), response.end(), response.begin(), ::toupper);
        send(clientSocket, response.c_str(), response.length(), 0);
    }
    closesocket(clientSocket);
}


int main() {
    // Design structure
    /*
    1. Creating the Server Socket || Server inputs: IP, port number
    2. Listen for connection on all ports
    3. Get the commands from the client
    4. Fetching data - do actions
    5. Send the response back to the client
    6. Don't quit
     */
    Server *server = new Server();
    server->startListening();
    return 0;
}
