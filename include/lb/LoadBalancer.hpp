#pragma once

#include "network/TcpServer.hpp"
#include "network/Connection.hpp"
#include "config/Config.hpp"
#include "core/EventLoop.hpp"
#include "lb/Backend.hpp"
#include "lb/Scheduler.hpp"
#include<memory>


class LoadBalancer
{
public:
    explicit LoadBalancer(const Config& config);

    bool initialize();
    void run();

private:
    Config config_;

    TcpServer server_;
    EventLoop eventLoop_;
    std::vector<epoll_event> events_;

    std::unordered_map<int, std::shared_ptr<Connection>> connections_;

    std::vector<Backend> backends_;
    std::unique_ptr<Scheduler> scheduler_;

    void acceptNewClient();

    void handleSocketEvent(int fd);

    void cleanupConnection(int fd);

};