//
// Created by phida on 10/9/2024.
//

#include "main.h"


using namespace std;

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
    server->StartListening();
    return 0;
}
