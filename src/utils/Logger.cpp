#include "utils/Loggers.hpp"

#include <chrono>
#include <ctime>
#include <iomanip>
#include <iostream>

void Logger::info(const std::string& message)
{
    log("INFO", message);
}

void Logger::warn(const std::string& message)
{
    log("WARN", message);
}

void Logger::error(const std::string& message)
{
    log("ERROR", message);
}

void Logger::log(const std::string& level,
                 const std::string& message)
{
    auto now = std::chrono::system_clock::now();
    auto now_time = std::chrono::system_clock::to_time_t(now);

    std::tm* local = std::localtime(&now_time);

    std::cout
        << "["
        << std::put_time(local, "%Y-%m-%d %H:%M:%S")
        << "] "
        << "["
        << level
        << "] "
        << message
        << std::endl;
}