#include "lb/Backend.hpp"

#include "utils/Loggers.hpp"

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

#include <netdb.h>
#include <cstring>

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
    addrinfo hints{};
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;

    addrinfo* result = nullptr;

    int ret = getaddrinfo(
        host_.c_str(),
        std::to_string(port_).c_str(),
        &hints,
        &result);

    if (ret != 0)
    {
        Logger::error(gai_strerror(ret));
        return -1;
    }

    int fd = socket(
        result->ai_family,
        result->ai_socktype,
        result->ai_protocol);

    if (fd < 0)
    {
        freeaddrinfo(result);
        return -1;
    }

    if (::connect(fd,
                  result->ai_addr,
                  result->ai_addrlen) < 0)
    {
        close(fd);
        freeaddrinfo(result);
        return -1;
    }

    freeaddrinfo(result);

    return fd;
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