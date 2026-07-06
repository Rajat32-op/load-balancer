#include <arpa/inet.h>
#include <iostream>
#include <netinet/in.h>
#include <string>
#include <sys/socket.h>
#include <thread>
#include <unistd.h>

void handleClient(int clientFd, int port)
{
    char buffer[4096];

    while (true)
    {
        ssize_t n = recv(clientFd, buffer, sizeof(buffer), 0);

        if (n <= 0)
            break;

        std::string response = "[Backend " +
                               std::to_string(port) +
                               "] ";

        response.append(buffer, n);

        send(clientFd,
             response.data(),
             response.size(),
             0);
    }

    std::cout << "Client disconnected\n";

    close(clientFd);
}

int main(int argc, char* argv[])
{
    int port = 9001;

    if (argc > 1)
        port = std::stoi(argv[1]);

    int serverFd = socket(AF_INET, SOCK_STREAM, 0);

    if (serverFd < 0)
    {
        std::cerr << "socket() failed\n";
        return 1;
    }

    int opt = 1;
    setsockopt(serverFd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(port);

    if (bind(serverFd, (sockaddr*)&addr, sizeof(addr)) < 0)
    {
        std::cerr << "bind() failed\n";
        return 1;
    }

    if (listen(serverFd, SOMAXCONN) < 0)
    {
        std::cerr << "listen() failed\n";
        return 1;
    }

    std::cout << "Echo server listening on port "
              << port
              << std::endl;

    while (true)
    {
        sockaddr_in client{};
        socklen_t len = sizeof(client);

        int clientFd = accept(serverFd,
                              (sockaddr*)&client,
                              &len);

        if (clientFd < 0)
            continue;

        char ip[INET_ADDRSTRLEN];

        inet_ntop(AF_INET,
                  &client.sin_addr,
                  ip,
                  sizeof(ip));

        std::cout << "Client connected: "
                  << ip
                  << ":"
                  << ntohs(client.sin_port)
                  << std::endl;

        std::thread(handleClient, clientFd, port).detach();
    }

    close(serverFd);

    return 0;
}