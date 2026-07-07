#pragma once
#include "lb/Backend.hpp"
#include <chrono>

class Connection{
public:
    Connection(int clientFd, int backendFd);

    int getClientFd();
    int getBackendFd();
    Backend *getBackend();
    void setBackend(Backend *backend);
    void close();
    std::chrono::steady_clock::time_point lastActivity() const;
    void updateActivity();


private:
    int clientFd_;
    int backendFd_;

    Backend *backend_;
    std::chrono::steady_clock::time_point lastActivity_;

};