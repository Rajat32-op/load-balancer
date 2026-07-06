#pragma once

#include <string>

class Backend
{
public:
    Backend(const std::string& host, int port);

    int connectBackend();

    void incrementConnections();

    void decrementConnections();

    int activeConnections() const;

    bool healthy() const;

    void setHealthy(bool);

private:
    std::string host_;
    int port_;
    int weight_;
    bool healthy_;
    int activeConnections_;
};