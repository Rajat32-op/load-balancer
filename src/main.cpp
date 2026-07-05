#include "config/Config.hpp"
#include "utils/Loggers.hpp"

int main()
{
    Logger::info("Starting Load Balancer...");

    Config config;

    if (!config.load("config.json"))
    {
        Logger::error("Failed to load configuration.");
        return 1;
    }

    Logger::info("Configuration loaded successfully.");

    Logger::info("Listening on " +
                 config.listenIp +
                 ":" +
                 std::to_string(config.listenPort));

    Logger::info("Algorithm: " + config.algorithm);

    for (const auto& backend : config.backends)
    {
        Logger::info(
            "Backend: " +
            backend.host +
            ":" +
            std::to_string(backend.port) +
            " (weight=" +
            std::to_string(backend.weight) +
            ")");
    }

    Logger::info("Initialization complete.");

    return 0;
}