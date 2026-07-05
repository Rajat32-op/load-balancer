#pragma once

class Connection
{
public:
    Connection(int clientFd, int backendFd);

    void proxy();
    void close();

private:
    int clientFd_;
    int backendFd_;
};