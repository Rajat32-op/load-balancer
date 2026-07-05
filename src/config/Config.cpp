#include "config/Config.hpp"

#include <fstream>
#include <iostream>

#include <nlohmann/json.hpp>

using json = nlohmann::json;

bool Config::load(const std::string& filename)
{
    std::ifstream file(filename);

    if (!file.is_open())
    {
        std::cerr << "Failed to open config file: " << filename << '\n';
        return false;
    }

    json j;
    file >> j;

    listenIp = j["listen_ip"];
    listenPort = j["listen_port"];
    algorithm = j["algorithm"];

    backends.clear();

    for (const auto& backend : j["backends"])
    {
        BackendConfig b;

        b.host = backend["host"];
        b.port = backend["port"];
        b.weight = backend["weight"];

        backends.push_back(b);
    }

    return true;
}