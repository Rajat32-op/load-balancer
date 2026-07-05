#pragma once

#include <string>

class TcpServer
{
public:
    TcpServer(const std::string& ip, int port);
    ~TcpServer();

    bool start();
    int acceptClient();
    void stop();
    int getListenFd();

private:
    std::string ip_;
    int port_;

    int listenFd_;
};