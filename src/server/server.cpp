#include "server.h"


// asio::io_context io_context;
// udp::socket multicast_socket(io_context);

Server::Server() = default;

Server::~Server()
{
    closesocket(server_socket);
    WSACleanup();
    std::cout << "Server destroyed" << endl;
}

bool Server::setupWSA()
{
    return WSAStartup(MAKEWORD(2, 2), &wsa_data) == 0;
}

bool Server::setupSocket()
{
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    return server_socket != INVALID_SOCKET;
}

bool Server::assignPort()
{
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = INADDR_ANY;
    server_address.sin_port = 0;

    if (bind(server_socket, reinterpret_cast<sockaddr *>(&server_address), sizeof(server_address)) == SOCKET_ERROR)
    {
        std::cerr << "Bind failed" << std::endl;
        closesocket(server_socket);
        WSACleanup();
        return false;
    }

    int len_address = sizeof(server_address);

    if (getsockname(server_socket, reinterpret_cast<sockaddr *>(&server_address), &len_address) == SOCKET_ERROR)
    {
        cerr << "Failed to get socket name" << endl;
        closesocket(server_socket);
        WSACleanup();
        return false;
    }
    assigned_port = ntohs(server_address.sin_port);
    return true;
}

bool Server::setupServer()
{
    if (!setupWSA())
    {
        WSACleanup();
        return false;
    }

    if (!setupSocket())
    {
        closesocket(server_socket);
        WSACleanup();
        return false;
    }

    return assignPort();
}

void Server::listOfCommands()
{
    wss << "Available commands:\n";
    wss << "\t!help\n";
    wss << "\t!list p\n";
    wss << "\t!list s\n";
    wss << "\t!screenshot\n";
    wss << "\t!webcam\n";
    wss << "\t!capture\n";
    wss << "\t!shutdown [1, 2, 3]\n";
    wss << "\t!end p [process_id]\n";
    wss << "\t!end s [service_name]\n";
    wss << "\t!exit\n";
}

void Server::listProcesses()
{
    std::vector<ProcessInfo> processes = Process::listProcesses();
    std::map<ProcessType, std::vector<ProcessInfo>> groupedProcesses;

    for (const auto &process : processes)
    {
        groupedProcesses[process.type].push_back(process);
    }

    const wchar_t *typeNames[] = {L"Apps", L"Background processes", L"Windows processes"};

    for (int i = 0; i < 3; ++i)
    {
        auto type = static_cast<ProcessType>(i);
        wss << typeNames[i] << L" (" << groupedProcesses[type].size() << L")\n";
        wss << std::wstring(50, L'-') << L"\n";

        for (const auto &process : groupedProcesses[type])
        {
            wss << std::left << std::setw(10) << process.pid << process.name << L"\n";
        }
        wss << L"\n";
    }
    wss << L"Total processes: " << processes.size() << L"\n";
}

void Server::listServices()
{
    std::vector<ServiceInfo> services = Service::listServices();

    wss << std::left << std::setw(40) << L"Service Name"
        << std::setw(50) << L"Display Name"
        << L"State\n";
    wss << std::wstring(100, L'-') << L"\n";

    for (const auto &service : services)
    {
        wss << std::left << std::setw(40) << service.name
            << std::setw(50) << service.displayName
            << getStateString(service.currentState) << L"\n";
    }
    wss << L"\nTotal services: " << services.size() << L"\n";
}

void Server::screenShot(std::vector<char> &image)
{
    image = WindowsCommands::screenShot();
    wss << L"Screen capture completed.\n";
}

void Server::toggleWebcam()
{
    if (webcam_controller.IsWebcamRunning())
    {
        webcam_controller.StopWebcam();
        wss << L"Webcam stopped.\n"; // TODO: Multithreading
    }
    else
    {
        webcam_controller.StartWebcam();
        wss << L"Webcam started.\n";
    }

}

void Server::shutdown(const char *buffer)
{
    char *endPtr;
    const UINT nSDType = strtol(buffer + 9, &endPtr, 10);

    WindowsCommands::shutdown(nSDType);
    wss << L"Shutdown completed.\n";
}

void Server::endProcess(const char *buffer)
{
    char *endPtr;
    const int app_id = strtol(buffer + 4, &endPtr, 10);
    if (*endPtr == '\0' || *endPtr == ' ')
    {
        std::cout << app_id << std::endl;
        wss << "Trying to close " << app_id << std::endl;
        if (!Process::endProcess(app_id))
            wss << "No such app with such ID" << std::endl;
    }
    else
        wss << "Invalid ID" << std::endl;
}

void Server::endService(const char *buffer)
{
    char serviceName[256] = {0};
    const char *start = buffer + 5;

    size_t len = 0;
    while (start[len] != '\0' && start[len] != '\n' && start[len] != ' ' && start[len] != '\r' && len < sizeof(serviceName) - 1)
    {
        serviceName[len] = start[len];
        len++;
    }
    serviceName[len] = '\0';

    wss << L"Trying to stop service: " << serviceName << std::endl;
    if (!Service::endService(serviceName))
    {
        wss << L"Failed to stop the service." << std::endl;
    }
}

void Server::capture(vector<char> &image)
{
    image = webcam_controller.GetCurrentFrame();
    wss << L"Capture completed.\n";
}

int Server::sendSizeAndResponse(const SOCKET &client_socket) const
{
    std::wstring wstr = wss.str();
    const std::string str(wstr.begin(), wstr.end());

    int responseSize = static_cast<int>(str.length());
    send(client_socket, reinterpret_cast<char *>(&responseSize), sizeof(int), 0);

    return send(client_socket, str.c_str(), static_cast<int>(str.length()), 0);
}

void Server::handleClient(const SOCKET client_socket)
{
    while (true)
    {
        wss = wstringstream(std::wstring());
        std::vector<char> image = {};

        char buffer[1024] = {};
        const int received_bytes = recv(client_socket, buffer, sizeof(buffer), 0);

        if (received_bytes <= 0)
        {
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

        else if (strcmp(buffer, "!screenshot") == 0)
            screenShot(image);

        else if (strcmp(buffer, "!webcam") == 0)
            toggleWebcam();

        else if (strcmp(buffer, "!shutdown ") == 0)
            shutdown(buffer);

        else if (strstr(buffer, "!endp ") != nullptr)
            endProcess(buffer);

        else if (strstr(buffer, "!ends ") == buffer)
            endService(buffer);

        else if (strcmp(buffer, "!capture") == 0)
            capture(image);

        else if (strcmp(buffer, "!exit") == 0)
            break;

        else
            wss << L"Unknown command.\n";

        const int sent_bytes = sendSizeAndResponse(client_socket);

        if (!sent_bytes)
            std::cerr << "Failed to send size." << std::endl;

        if (strcmp(buffer, "!screenshot") == 0 || strcmp(buffer, "!capture") == 0)
        {
            int image_size = static_cast<int>(image.size());
            send(client_socket, reinterpret_cast<char *>(&image_size), sizeof(int), 0);

            if (!image.empty())
                send(client_socket, image.data(), static_cast<int>(image.size()), 0);

            if (sent_bytes == SOCKET_ERROR)
            {
                std::cerr << "send failed with error: " << WSAGetLastError() << std::endl;
                break;
            }
            std::cout << "Sent image data of size: " << image_size << " bytes" << std::endl;
        }
    }
    closesocket(client_socket);
}

void Server::startServer()
{
    if (!setupServer())
    {
        std::cerr << "Failed to setup server." << std::endl;
        return;
    }

    if (listen(server_socket, SOMAXCONN) == SOCKET_ERROR)
    {
        std::cerr << "Listen failed" << std::endl;
        closesocket(server_socket);
        WSACleanup();
        return;
    }
    cout << "Server listening for discovery on port " << assigned_port << std::endl;

    while (true)
    {
        DiscoveryResponder responder;
        responder.listen();

        while (true)
        {
            const SOCKET client_socket = accept(server_socket, nullptr, nullptr);
            if (client_socket == INVALID_SOCKET)
            {
                cerr << "Accept failed" << endl;
                continue;
            }
            cout << "New client connected." << endl;
            handleClient(client_socket);
            break;
        }
    }

}