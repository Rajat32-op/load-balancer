#pragma once

class Connection{
public:
    Connection(int clientFd, int backendFd);

    int getClientFd();
    int getBackendFd();
    void close();

private:
    int clientFd_;
    int backendFd_;
};