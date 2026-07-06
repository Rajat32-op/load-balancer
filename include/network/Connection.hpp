#pragma once
#include "lb/Backend.hpp"

class Connection{
public:
    Connection(int clientFd, int backendFd);

    int getClientFd();
    int getBackendFd();
    Backend *getBackend();
    void setBackend(Backend *backend);
    void close();

private:
    int clientFd_;
    int backendFd_;

    Backend *backend_;
};