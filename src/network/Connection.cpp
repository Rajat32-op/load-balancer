#include "network/Connection.hpp"

#include "utils/Loggers.hpp"

#include <sys/socket.h>
#include <unistd.h>

Connection::Connection(int clientFd, int backendFd)
    : clientFd_(clientFd),
      backendFd_(backendFd)
{
}

void Connection::proxy()
{
    constexpr int BUFFER_SIZE = 4096;
    char buffer[BUFFER_SIZE];

    while (true)
    {
        // Client -> Backend
        ssize_t bytesRead = recv(clientFd_, buffer, BUFFER_SIZE, 0);

        if (bytesRead <= 0)
        {
            Logger::info("Client disconnected.");
            break;
        }

        send(backendFd_, buffer, bytesRead, 0);

        // Backend -> Client
        ssize_t bytesReceived = recv(backendFd_, buffer, BUFFER_SIZE, 0);

        if (bytesReceived <= 0)
        {
            Logger::info("Backend disconnected.");
            break;
        }

        send(clientFd_, buffer, bytesReceived, 0);
    }
}

void Connection::close()
{
    if (clientFd_ != -1)
    {
        ::close(clientFd_);
        clientFd_ = -1;
    }

    if (backendFd_ != -1)
    {
        ::close(backendFd_);
        backendFd_ = -1;
    }
}