#include "lb/LoadBalancer.hpp"
#include "utils/Loggers.hpp"
#include "lb/Backend.hpp"
#include <unistd.h>
#include <netinet/in.h>

LoadBalancer::LoadBalancer(const Config& config)
    : config_(config),
      server_(config.listenIp, config.listenPort)
{
}

bool LoadBalancer::initialize()
{
    if (!server_.start())
    {
        Logger::error("Failed to start server.");
        return false;
    }

    if (!eventLoop_.addFD(server_.getListenFd(), EPOLLIN))
    {
        Logger::error("Failed to register listening socket with epoll.");
        return false;
    }

    return true;
}

void LoadBalancer::acceptNewClient()
{
    int clientFd = server_.acceptClient();
    if (clientFd == -1)
    {
        Logger::error("Failed to accept new client.");
        return;
    }

    Backend backend(
        config_.backends[0].host,
        config_.backends[0].port);

    if (!backend.connectBackend())
    {
        Logger::error("Failed to connect to backend server.");
        close(clientFd);
        return;
    }

    auto connection = std::make_shared<Connection>(clientFd, backend.getSocket());
    connections_[clientFd] = connection;
    connections_[backend.getSocket()] = connection;

    // Register both client and backend FDs with the event loop.
    eventLoop_.addFD(clientFd, EPOLLIN);
    eventLoop_.addFD(backend.getSocket(), EPOLLIN);
}

void LoadBalancer::handleSocketEvent(int fd)
{
    auto it = connections_.find(fd);
    if (it == connections_.end())
    {
        Logger::warn("Received event for unknown FD: " + std::to_string(fd));
        return;
    }

    auto connection = it->second;
    int fromFd,toFd;
    if(fd==connection->getClientFd()){
        fromFd=connection->getClientFd();
        toFd=connection->getBackendFd();
    }
    else{
        fromFd=connection->getBackendFd();
        toFd=connection->getClientFd();
    }

    char buffer[4096];
    ssize_t n = recv(fromFd,buffer,sizeof(buffer),0);

    if(n <= 0)
    {
        Logger::info("nothing received");
        cleanupConnection(fd);
    }
    else
    {
        printf("sending %s to %d\n", buffer, toFd);
        send(toFd,buffer,n,0);
    }
}

void LoadBalancer::cleanupConnection(int fd)
{
    auto it = connections_.find(fd);
    if (it != connections_.end())
    {
        auto connection = it->second;
        int clientFd = connection->getClientFd();
        int backendFd = connection->getBackendFd();

        eventLoop_.removeFD(clientFd);
        eventLoop_.removeFD(backendFd);

        connection->close();

        connections_.erase(clientFd);
        connections_.erase(backendFd);
    }
}

void LoadBalancer::run(){

    while (true)
    {
        eventLoop_.wait(events_);

        for (auto& event : events_)
        {
            if (event.data.fd == server_.getListenFd())
                acceptNewClient();
            else{
                handleSocketEvent(event.data.fd);
            }
        }
    }
}