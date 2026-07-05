#include "config/Config.hpp"
#include "core/EventLoop.hpp"
#include "network/TcpServer.hpp"
#include "utils/Loggers.hpp"

#include <sys/epoll.h>
#include <unistd.h>
#include <vector>

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

    if (!eventLoop.addFD(server.getListenFd(), EPOLLIN))
    {
        Logger::error("Failed to register listening socket with epoll.");
        return 1;
    }

    Logger::info("Event loop started.");

    std::vector<epoll_event> events;

    while (true)
    {
        int numEvents = eventLoop.wait(events);

        if (numEvents < 0)
        {
            Logger::error("epoll_wait() failed.");
            break;
        }

        for (const auto& event : events)
        {
            if (event.data.fd == server.getListenFd())
            {
                int clientFd = server.acceptClient();

                if (clientFd >= 0)
                {
                    Logger::info("Accepted client.");

                    close(clientFd);   // We'll keep it open in Phase 2.2
                }
            }
        }
    }

    return 0;
}