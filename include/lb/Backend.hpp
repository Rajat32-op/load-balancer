#pragma once

#include <string>

class Backend
{
public:
    Backend(const std::string& host, int port);

    bool connectBackend();
    void disconnect();

    int getSocket() const;

private:
    std::string host_;
    int port_;

    int socketFd_;
};