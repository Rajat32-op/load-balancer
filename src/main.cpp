#include "config/Config.hpp"
#include "core/EventLoop.hpp"
#include "network/TcpServer.hpp"
#include "network/Connection.hpp"
#include "lb/Backend.hpp"
#include "utils/Loggers.hpp"

#include <sys/epoll.h>
#include <unistd.h>
#include <vector>
#include <netinet/in.h>

int main()
{
    Config config;

    if (!config.load("config.json"))
    {
        Logger::error("Failed to load configuration.");
        return 1;
    }

    TcpServer server(config.listenIp, config.listenPort);

    if (!server.start())
    {
        Logger::error("Failed to start server.");
        return 1;
    }

    EventLoop eventLoop;

    std::unordered_map<int, Connection*> connections;
    std::vector<epoll_event> events;

    if (!eventLoop.addFD(server.getListenFd(), EPOLLIN)){
        Logger::error("Failed to register listening socket with epoll.");
        return 1;
    }

    Logger::info("Event loop started.");


    int listenFd=server.getListenFd();
    while (true)
{
    eventLoop.wait(events);

    for (auto &event : events)
    {
        int fd = event.data.fd;

        if (fd == listenFd)
        {
            int clientFd = server.acceptClient();
            Backend backend(
            config.backends[0].host,
            config.backends[0].port);

            backend.connectBackend();

            Connection* conn =
                new Connection(
                    clientFd,
                    backend.getSocket());

            connections[clientFd] = conn;
            connections[backend.getSocket()] = conn;

            eventLoop.addFD(clientFd, EPOLLIN);
            eventLoop.addFD(backend.getSocket(), EPOLLIN);
            
        }
        else
        {
            char buffer[4096];
            int fd=event.data.fd;
            Connection *conn=connections[fd];
            int fromFd,toFd;
            if (fd == conn->getClientFd())
            {
                fromFd = conn->getClientFd();
                toFd = conn->getBackendFd();
            }
            else
            {
                fromFd = conn->getBackendFd();
                toFd = conn->getClientFd();
            }
            ssize_t n = recv(
                fromFd,
                buffer,
                sizeof(buffer),
                0);

            if(n <= 0)
            {
                eventLoop.removeFD(fromFd);
                close(fromFd);
            }
            else
            {
                send(toFd,
                    buffer,
                    n,
                    0);
            }
            
        }
    }
}

    return 0;
}