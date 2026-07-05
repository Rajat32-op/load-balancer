#include "config/Config.hpp"
#include "utils/Loggers.hpp"
#include "lb/Backend.hpp"
#include "network/Connection.hpp"
#include "network/TcpServer.hpp"
#include <unistd.h>

int main()
{
    Logger::info("Starting Load Balancer...");

    Config config;

    if (!config.load("config.json"))
    {
        Logger::error("Failed to load configuration.");
        return 1;
    }

    TcpServer server(config.listenIp,config.listenPort);
    if(!server.start()){
        return 1;
    }

   while (true)
{
    int clientFd = server.acceptClient();

    if (clientFd < 0)
        continue;
    
    Backend backend(
        config.backends[0].host,
        config.backends[0].port);

    if (!backend.connectBackend())
    {
        ::close(clientFd);
        continue;
    }

    Connection connection(
        clientFd,
        backend.getSocket());

    connection.proxy();
    connection.close();
}

    return 0;
}