#pragma once
#include "lb/Backend.hpp"
#include <vector>
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
    std::vector<char>& clientToBackendBuffer();
    std::vector<char>& backendToClientBuffer();


private:
    int clientFd_;
    int backendFd_;

    Backend *backend_;
    std::chrono::steady_clock::time_point lastActivity_;

    std::vector<char> clientToBackendBuffer_;
    std::vector<char> backendToClientBuffer_;

};