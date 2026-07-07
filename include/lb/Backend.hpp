#pragma once

#include <string>
#include <atomic>
#include <memory>
class Backend
{
public:
    Backend(const std::string& host, int port, int weight);

    int connectBackend();

    void incrementConnections();

    void decrementConnections();

    int activeConnections() const;

    bool healthy() const;

    void setHealthy(bool);

    int weight() const;

private:
    std::string host_;
    int port_;
    int weight_;
    std::atomic<bool> healthy_;
    std::atomic<int> activeConnections_;
};