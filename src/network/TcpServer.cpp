#include "network/TcpServer.hpp"

#include "utils/Loggers.hpp"

#include <arpa/inet.h>
#include <cstring>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <fcntl.h>

TcpServer::TcpServer(const std::string& ip, int port)
    : ip_(ip),
      port_(port),
      listenFd_(-1)
{
}

TcpServer::~TcpServer()
{
    stop();
}

bool TcpServer::start()
{
    listenFd_ = socket(AF_INET, SOCK_STREAM, 0);

    if (listenFd_ < 0)
    {
        Logger::error("Failed to create socket.");
        return false;
    }

    setNonBlocking(listenFd_);

    int opt = 1;
    setsockopt(listenFd_, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    sockaddr_in serverAddr{};
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(port_);

    if (inet_pton(AF_INET, ip_.c_str(), &serverAddr.sin_addr) <= 0)
    {
        Logger::error("Invalid IP address.");
        return false;
    }

    if (bind(listenFd_,
             reinterpret_cast<sockaddr*>(&serverAddr),
             sizeof(serverAddr)) < 0)
    {
        Logger::error("bind() failed.");
        return false;
    }

    if (listen(listenFd_, SOMAXCONN) < 0)
    {
        Logger::error("listen() failed.");
        return false;
    }

    Logger::info("Listening on " + ip_ + ":" + std::to_string(port_));

    return true;
}

int TcpServer::acceptClient()
{
    sockaddr_in clientAddr{};
    socklen_t clientLen = sizeof(clientAddr);

    int clientFd = accept(
        listenFd_,
        reinterpret_cast<sockaddr*>(&clientAddr),
        &clientLen);

    if (clientFd < 0)
    {
        return -1;
    }

    char ip[INET_ADDRSTRLEN];

    inet_ntop(AF_INET,
              &clientAddr.sin_addr,
              ip,
              sizeof(ip));

    Logger::info(
        "Client connected: " +
        std::string(ip) +
        ":" +
        std::to_string(ntohs(clientAddr.sin_port)));

    return clientFd;
}

int TcpServer::getListenFd(){
    return listenFd_;
}
void TcpServer::stop()
{
    if (listenFd_ != -1)
    {
        close(listenFd_);
        listenFd_ = -1;
    }
}

bool setNonBlocking(int fd){
    int flags = fcntl(fd, F_GETFL, 0);

    if (flags == -1)
        return false;

    return fcntl(
            fd,
            F_SETFL,
            flags | O_NONBLOCK) != -1;
}