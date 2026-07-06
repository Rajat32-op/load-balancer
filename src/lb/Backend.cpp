#include "lb/Backend.hpp"

#include "utils/Loggers.hpp"

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

Backend::Backend(const std::string& host, int port)
    : host_(host),
      port_(port),
      weight_(1),
      healthy_(true),
      activeConnections_(0)
{
}

int Backend::connectBackend()
{
    int socketFd_ = socket(AF_INET, SOCK_STREAM, 0);

    if (socketFd_ < 0)
    {
        Logger::error("Failed to create backend socket.");
        return -1;
    }

    sockaddr_in serverAddr{};
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(port_);

    if (inet_pton(AF_INET, host_.c_str(), &serverAddr.sin_addr) <= 0)
    {
        Logger::error("Invalid backend IP.");
        close(socketFd_);
        socketFd_ = -1;
        return -1;
    }

    if (::connect(socketFd_,
                  reinterpret_cast<sockaddr*>(&serverAddr),
                  sizeof(serverAddr)) < 0)
    {
        Logger::error("Failed to connect to backend.");
        close(socketFd_);
        socketFd_ = -1;
        return -1;
    }

    Logger::info("Connected to backend " +
                 host_ +
                 ":" +
                 std::to_string(port_));

    return socketFd_;
}