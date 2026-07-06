#pragma once

#include <string>

class Backend
{
public:
    Backend(const std::string& host, int port);

    int connectBackend();

private:
    std::string host_;
    int port_;
    int weight_;

};