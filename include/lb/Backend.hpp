#pragma once

#include <string>

class Backend
{
public:
    Backend(const std::string& host, int port);

    int connectBackend();
    void disconnect();

private:
    std::string host_;
    int port_;

};