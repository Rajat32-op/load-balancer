#include "lb/Backend.hpp"

#include "utils/Loggers.hpp"

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

Backend::Backend(const std::string& host, int port,int weight)
    : host_(host),
      port_(port),
      weight_(weight)
{
    healthy_.store(true);
    activeConnections_.store(0);
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

bool Backend::healthy() const
{
    return healthy_.load();
}

void Backend::setHealthy(bool healthy)
{
    healthy_.store(healthy);
}

int Backend::weight() const
{
    return weight_;
}

void Backend::incrementConnections()
{
    activeConnections_.fetch_add(1);
}

void Backend::decrementConnections()
{
    if (activeConnections_ > 0)
    {
        activeConnections_.fetch_sub(1);
    }
}

int Backend::activeConnections() const
{
    return activeConnections_.load();
}