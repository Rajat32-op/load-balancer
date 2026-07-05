#include "lb/Backend.hpp"

#include "utils/Loggers.hpp"

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

Backend::Backend(const std::string& host, int port)
    : host_(host),
      port_(port),
      socketFd_(-1)
{
}

bool Backend::connectBackend()
{
    socketFd_ = socket(AF_INET, SOCK_STREAM, 0);

    if (socketFd_ < 0)
    {
        Logger::error("Failed to create backend socket.");
        return false;
    }

    sockaddr_in serverAddr{};
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(port_);

    if (inet_pton(AF_INET, host_.c_str(), &serverAddr.sin_addr) <= 0)
    {
        Logger::error("Invalid backend IP.");
        close(socketFd_);
        socketFd_ = -1;
        return false;
    }

    if (::connect(socketFd_,
                  reinterpret_cast<sockaddr*>(&serverAddr),
                  sizeof(serverAddr)) < 0)
    {
        Logger::error("Failed to connect to backend.");
        close(socketFd_);
        socketFd_ = -1;
        return false;
    }

    Logger::info("Connected to backend " +
                 host_ +
                 ":" +
                 std::to_string(port_));

    return true;
}

void Backend::disconnect()
{
    if (socketFd_ != -1)
    {
        close(socketFd_);
        socketFd_ = -1;
    }
}

int Backend::getSocket() const
{
    return socketFd_;
}