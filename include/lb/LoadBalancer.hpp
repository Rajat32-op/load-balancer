#pragma once

#include "network/TcpServer.hpp"
#include "network/Connection.hpp"
#include "config/Config.hpp"
#include "core/EventLoop.hpp"
#include "lb/Backend.hpp"
#include "lb/Scheduler.hpp"
#include "core/HealthChecker.hpp"

class LoadBalancer
{
public:
    explicit LoadBalancer(const Config& config);
    ~LoadBalancer();

    bool initialize();
    void run();

private:
    Config config_;

    TcpServer server_;
    EventLoop eventLoop_;
    std::vector<epoll_event> events_;

    std::unordered_map<int, std::shared_ptr<Connection>> connections_;

    std::vector<std::unique_ptr<Backend>> backends_;
    std::unique_ptr<Scheduler> scheduler_;
    std::unique_ptr<HealthChecker> healthChecker_;

    const std::chrono::seconds IDLE_TIMEOUT = std::chrono::seconds(60);

    void acceptNewClient();

    void handleSocketEvent(int fd);

    void cleanupConnection(int fd);

    void checkIdleConnections();

};