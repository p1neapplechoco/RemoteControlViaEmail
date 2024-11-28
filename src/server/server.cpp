#include "server.h"


// asio::io_context io_context;
// udp::socket multicast_socket(io_context);

Server::Server() = default;

Server::~Server() {
    closesocket(server_socket);
    WSACleanup();
    std::cout << "Server destroyed" << endl;
}

bool Server::setupWSA() {
    return WSAStartup(MAKEWORD(2, 2), &wsa_data) == 0;
}

bool Server::setupSocket() {
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    return server_socket != INVALID_SOCKET;
}

bool Server::assignPort() {
    const int DEFAULT_PORT = 42069;

    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = INADDR_ANY;
    server_address.sin_port = htons(DEFAULT_PORT);

    if (bind(server_socket, reinterpret_cast<sockaddr *>(&server_address), sizeof(server_address)) == SOCKET_ERROR) {
        std::cerr << "Bind failed" << std::endl;
        closesocket(server_socket);
        WSACleanup();
        return false;
    }

    int len_address = sizeof(server_address);

    if (getsockname(server_socket, reinterpret_cast<sockaddr *>(&server_address), &len_address) == SOCKET_ERROR) {
        cerr << "Failed to get socket name" << endl;
        closesocket(server_socket);
        WSACleanup();
        return false;
    }
    assigned_port = ntohs(server_address.sin_port);
    return true;
}

bool Server::setupServer() {
    if (!setupWSA()) {
        WSACleanup();
        return false;
    }

    if (!setupSocket()) {
        closesocket(server_socket);
        WSACleanup();
        return false;
    }

    return assignPort();
}

void Server::listOfCommands() {
    wss << "Available commands:\n";
    wss << "\t!help\n";
    wss << "\t!list p\n";
    wss << "\t!list s\n";
    wss << "\t!screenshot\n";
    wss << "\t!webcam\n";
    wss << "\t!capture\n";
    wss << "\t!shutdown [0, 1, 2]\n";
    wss << "\t!list disks\n";
    wss << "\t!index <disks/''>\n";
    wss << "\t!get file <file_path>\n";
    wss << "\t!endp [process_id]\n";
    wss << "\t!ends [service_name]\n";
    wss << "\t!exit\n";
}

void Server::listProcesses() {
    std::vector<ProcessInfo> processes = Process::listProcesses();
    std::map<ProcessType, std::vector<ProcessInfo> > groupedProcesses;

    for (const auto &process: processes) {
        groupedProcesses[process.type].push_back(process);
    }

    const wchar_t *typeNames[] = {L"Apps", L"Background processes", L"Windows processes"};

    for (int i = 0; i < 3; ++i) {
        auto type = static_cast<ProcessType>(i);
        wss << typeNames[i] << L" (" << groupedProcesses[type].size() << L")\n";
        wss << std::wstring(50, L'-') << L"\n";

        for (const auto &process: groupedProcesses[type]) {
            wss << std::left << std::setw(10) << process.pid << process.name << L"\n";
        }
        wss << L"\n";
    }
    wss << L"Total processes: " << processes.size() << L"\n";
}

void Server::listServices() {
    std::vector<ServiceInfo> services = Service::listServices();

    wss << std::left << std::setw(40) << L"Service Name"
            << std::setw(50) << L"Display Name"
            << L"State\n";
    wss << std::wstring(100, L'-') << L"\n";

    for (const auto &service: services) {
        wss << std::left << std::setw(40) << service.name
                << std::setw(50) << service.displayName
                << getStateString(service.currentState) << L"\n";
    }
    wss << L"\nTotal services: " << services.size() << L"\n";
}

void Server::screenShot(std::vector<char> &image) {
    image = WindowsCommands::screenShot();
    wss << L"Screen capture completed.\n";
}

void Server::toggleWebcam() {
    if (webcam_controller.IsWebcamRunning()) {
        webcam_controller.StopWebcam();
        wss << L"Webcam stopped.\n"; // TODO: Multithreading
    } else {
        webcam_controller.StartWebcam();
        wss << L"Webcam started.\n";
    }
}

void Server::Shutdown(const char *buffer) {
    char *endPtr;
    const UINT nSDType = strtol(buffer + 9, &endPtr, 10);

    WindowsCommands::shutdown(nSDType);
    wss << L"Shutdown completed.\n";
}

void Server::endProcess(const char *buffer) {
    char *endPtr;
    const int app_id = strtol(buffer + 5, &endPtr, 10);
    if (*endPtr == '\0' || *endPtr == ' ') {
        std::cout << app_id << std::endl;
        wss << "Trying to close " << app_id << std::endl;
        if (!Process::endProcess(app_id))
            wss << "No such app with such ID" << std::endl;
    } else
        wss << "Invalid ID" << std::endl;
}

void Server::endService(const char *buffer) {
    char serviceName[256] = {0};
    const char *start = buffer + 6;

    size_t len = 0;
    while (start[len] != '\0' && start[len] != '\n' && start[len] != ' ' && start[len] != '\r' && len < sizeof(
               serviceName) - 1) {
        serviceName[len] = start[len];
        len++;
    }
    serviceName[len] = '\0';

    wss << L"Trying to stop service: " << serviceName << std::endl;
    if (!Service::endService(serviceName)) {
        wss << L"Failed to stop the service." << std::endl;
    }
}

void Server::capture(vector<char> &image) {
    image = webcam_controller.GetCurrentFrame();
    wss << L"Capture completed.\n";
}

// void Server::openAndSendFile(string file_path)
// {
//     openFile(file_path);
// }


int Server::sendSizeAndResponse(const SOCKET &client_socket) const {
    std::wstring wstr = wss.str();
    const std::string str(wstr.begin(), wstr.end());

    int responseSize = static_cast<int>(str.length());
    send(client_socket, reinterpret_cast<char *>(&responseSize), sizeof(int), 0);

    return send(client_socket, str.c_str(), static_cast<int>(str.length()), 0);
}

void Server::handleClient(const SOCKET client_socket) {
    while (true) {
        wss = wstringstream(std::wstring());
        std::vector<char> image = {};

        char buffer[1024] = {};
        const int received_bytes = recv(client_socket, buffer, sizeof(buffer), 0);

        if (received_bytes <= 0) {
            std::cout << "Client disconnected" << std::endl;
            closesocket(client_socket);
            return;
        }
        std::cout << "Received: " << buffer << std::endl;

        std::string response;

        if (strcmp(buffer, "!help") == 0)
            listOfCommands();

        else if (strcmp(buffer, "!list p") == 0)
            listProcesses();

        else if (strcmp(buffer, "!list s") == 0)
            listServices();

        else if (strcmp(buffer, "!screenshot") == 0) {
            screenShot(image);
            SendImage(image, client_socket);
        } else if (strcmp(buffer, "!webcam") == 0)
            toggleWebcam();

        else if (strcmp(buffer, "!shutdown ") == 0)
            Shutdown(buffer);

        else if (strstr(buffer, "!endp ") != nullptr)
            endProcess(buffer);

        else if (strstr(buffer, "!ends ") != nullptr)
            endService(buffer);

        else if (strcmp(buffer, "!capture") == 0) {
            capture(image);
            SendImage(image, client_socket);
        } else if (strcmp(buffer, "!list disks") == 0)
            ShowAvailableDisks();

        else if (strstr(buffer, "!index") != NULL)
            IndexSystem(string(buffer + 7), client_socket);

        else if (strstr(buffer, "!get file") != NULL) {
            GetAndSendFile(string(buffer + 10), client_socket);
        } else if (strcmp(buffer, "!exit") == 0)
            break;

        else
            wss << L"Unknown command.\n";

        const int sent_bytes = sendSizeAndResponse(client_socket);

        if (!sent_bytes)
            std::cerr << "Failed to send size." << std::endl;

        // if (strcmp(buffer, "!screenshot") == 0 || strcmp(buffer, "!capture") == 0) {
        //     int image_size = static_cast<int>(image.size());
        //     send(client_socket, reinterpret_cast<char *>(&image_size), sizeof(int), 0);
        //
        //     if (!image.empty())
        //         send(client_socket, image.data(), static_cast<int>(image.size()), 0);
        //
        //     // if (sent_bytes == SOCKET_ERROR) {
        //     //     std::cerr << "send failed with error: " << WSAGetLastError() << std::endl;
        //     //     break;
        //     // }
        //     std::cout << "Sent image data of size: " << image_size << " bytes" << std::endl;
        // } else if (strstr(buffer, "!get file")) {
        //     int file_size = static_cast<int>(fileData.size());
        //     send(client_socket, reinterpret_cast<char *>(&file_size), sizeof(int), 0);
        //
        //     if (!fileData.empty())
        //         send(client_socket, fileData.data(), static_cast<int>(fileData.size()), 0);
        //
        //     // if (sent_bytes == SOCKET_ERROR) {
        //     //     std::cerr << "send failed with error: " << WSAGetLastError() << std::endl;
        //     //     break;
        //     // }
        //     std::cout << "Sent file of size: " << file_size << " bytes" << std::endl;
        // }
    }
    closesocket(client_socket);
}

void Server::SendImage(vector<char> &image, const SOCKET client_socket) {
    int image_size = static_cast<int>(image.size());
    send(client_socket, reinterpret_cast<char *>(&image_size), sizeof(int), 0);

    if (!image.empty())
        send(client_socket, image.data(), static_cast<int>(image.size()), 0);

    std::cout << "Sent image data of size: " << image_size << " bytes" << std::endl;
}

void Server::startServer() {
    if (!setupServer()) {
        std::cerr << "Failed to setup server." << std::endl;
        return;
    }

    if (listen(server_socket, SOMAXCONN) == SOCKET_ERROR) {
        std::cerr << "Listen failed" << std::endl;
        closesocket(server_socket);
        WSACleanup();
        return;
    }
    cout << "Server listening for discovery on port " << assigned_port << std::endl;

    while (true) {
        DiscoveryResponder responder;

        // Set up timeout for responder.listen()
        fd_set readfds;
        struct timeval listen_timeout;
        listen_timeout.tv_sec = LISTEN_TIMEOUT_SECONDS;
        listen_timeout.tv_usec = 0;

        // Start listening
        try {
            responder.listen();

            while (true) {
                // Set up file descriptor set and timeout for accept
                FD_ZERO(&readfds);
                FD_SET(server_socket, &readfds);

                struct timeval connection_timeout;
                connection_timeout.tv_sec = CONNECTION_TIMEOUT_SECONDS;
                connection_timeout.tv_usec = 0;

                // Wait for connection with timeout
                int activity = select(server_socket + 1, &readfds, NULL, NULL, &connection_timeout);

                if (activity == 0) {
                    // Timeout occurred
                    cout << "Connection timeout, restarting listener..." << endl;
                    break; // Break inner loop to restart responder
                } else if (activity < 0) {
                    cerr << "Select error" << endl;
                    break;
                }

                // If we get here, we have a pending connection
                const SOCKET client_socket = accept(server_socket, nullptr, nullptr);
                if (client_socket == INVALID_SOCKET) {
                    cerr << "Accept failed" << endl;
                    continue;
                }

                cout << "New client connected." << endl;
                handleClient(client_socket);
                break;
            }
        } catch (const std::exception &e) {
            cerr << "Listener error: " << e.what() << endl;
        }

        // Wait before restarting the responder
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
}

void Server::ShowAvailableDisks() {
    wss << L"Available Disks: " << std::endl;

    for (const auto &disk: getWinDir.listDisks())
        wss << disk.c_str() << std::endl;
}

void Server::IndexSystem(string drive, const SOCKET clientSocket) {
    if (drive.empty()) {
        wss << "Scanned disks: " << std::endl;

        // Send numbers of disks
        int file_size = static_cast<int>(getWinDir.disks.size());
        send(clientSocket, reinterpret_cast<char *>(&file_size), sizeof(int), 0);

        for (const auto &disk: getWinDir.disks) {
            std::string fileName = "cache_" + std::string(1, disk[0]) + ".txt";
            std::ofstream file(fileName);
            wss << disk.c_str() << std::endl;
            if (!file.is_open()) {
                std::cerr << "Error opening file " << fileName << std::endl;
                continue;
            }

            file << disk << '\\' << std::endl;
            GetWinDirectory::fullScan(disk, file);

            file.close();

            // Send disk ID
            send(clientSocket, disk.c_str(), static_cast<int>(disk.size()), 0);

            GetAndSendFile(fileName, clientSocket);
            std::cout << "Full scan of " << disk << " has been written to " << fileName << std::endl;
        }
    } else {
        wss << "Scanned " << drive.c_str() << std::endl;
        std::string fileName = "cache_" + std::string(1, drive[0]) + ".txt";
        std::ofstream file(fileName);
        if (!file.is_open()) {
            std::cerr << "Error opening file " << fileName << std::endl;
            return;
        }

        file << drive << '\\' << std::endl;
        GetWinDirectory::fullScan(drive, file);

        file.close();
        GetAndSendFile(fileName, clientSocket);
        std::cout << "Full scan of " << drive << " has been written to " << fileName << std::endl;
    }
}

void Server::GetAndSendFile(string filePath, const SOCKET client_socket) {
    if (filePath.size() > 4 && filePath.substr(filePath.size() - 4) == ".exe")
    {
        wss << "Opening .exe file: " << filePath.c_str() << std::endl;
        ShellExecuteA(nullptr, "open", filePath.c_str(), nullptr, nullptr, SW_SHOWNORMAL);
        return;
    }
    std::vector<char> fileData;
    try {
        std::ifstream file(filePath, std::ios::binary);
        if (!file) {
            throw std::runtime_error("Cannot open file");
        }

        // Get file size
        file.seekg(0, std::ios::end);
        std::streamsize size = file.tellg();
        file.seekg(0, std::ios::beg);
        // Read file into vector

        std::vector<char> buffer(size);
        if (!file.read(buffer.data(), size)) {
            throw std::runtime_error("Failed to read file");
        }

        fileData = buffer;
        wss << L"File sent: " << filePath.c_str() << std::endl;
    } catch (const std::exception &e) {
        std::cerr << "Error reading file: " << e.what() << std::endl;
        fileData = std::vector<char>();
        wss << L"Error reading file: " << e.what() << std::endl;
    }

    int file_size = static_cast<int>(fileData.size());
    send(client_socket, reinterpret_cast<char *>(&file_size), sizeof(int), 0);

    if (!fileData.empty())
        send(client_socket, fileData.data(), static_cast<int>(fileData.size()), 0);

    std::cout << "Sent file of size: " << file_size << " bytes" << std::endl;
};
