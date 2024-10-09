//
// Created by phida on 10/9/2024.
//

#include <iostream>
#include "server.h"

using namespace std;

Server::Server() {
    cout << "Server created" << endl;
}

Server::~Server() {
    cout << "Server destroyed" << endl;
}



void Server::start() {
    cout << "Server started" << endl;
}

void Server::stop() {
    cout << "Server stopped" << endl;
}

void Server::printIP() {
    cout << "Server IP:";
}

