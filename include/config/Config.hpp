#pragma once

#include <string>
#include <vector>

struct BackendConfig
{
    std::string host;
    int port;
    int weight;
};

class Config
{
public:
    bool load(const std::string& filename);

    std::string listenIp;
    int listenPort;

    std::string algorithm;

    std::vector<BackendConfig> backends;
};