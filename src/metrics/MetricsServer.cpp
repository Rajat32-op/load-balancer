#include "metrics/MetricsServer.hpp"

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

MetricsServer::MetricsServer(Metrics& metrics)
    : metrics_(metrics),
      running_(false)
{
}

void MetricsServer::start()
{
    running_ = true;
    worker_ = std::thread(&MetricsServer::run, this);
}

void MetricsServer::stop()
{
    running_ = false;

    if(worker_.joinable())
        worker_.join();
}

void MetricsServer::run()
{
    int serverFd = socket(AF_INET, SOCK_STREAM, 0);

    int opt = 1;
    setsockopt(serverFd,
               SOL_SOCKET,
               SO_REUSEADDR,
               &opt,
               sizeof(opt));

    sockaddr_in addr{};

    addr.sin_family = AF_INET;
    addr.sin_port = htons(9090);
    addr.sin_addr.s_addr = INADDR_ANY;

    bind(serverFd,
         (sockaddr*)&addr,
         sizeof(addr));

    listen(serverFd, SOMAXCONN);

    while(running_)
    {
        int clientFd = accept(serverFd,
                              nullptr,
                              nullptr);

        if(clientFd < 0)
            continue;

        char buffer[1024];

        recv(clientFd,
             buffer,
             sizeof(buffer),
             0);

        std::string body = metrics_.toJson();

        std::string response =
            "HTTP/1.1 200 OK\r\n"
            "Content-Type: application/json\r\n"
            "Content-Length: " +
            std::to_string(body.size()) +
            "\r\n\r\n" +
            body;

        send(clientFd,
             response.c_str(),
             response.size(),
             0);

        close(clientFd);
    }

    close(serverFd);
}