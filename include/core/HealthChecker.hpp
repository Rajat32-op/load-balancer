#pragma once
#include "lb/Backend.hpp"
#include <thread>
#include <atomic>
#include <vector>

class HealthChecker
{
public:
    HealthChecker(std::vector<std::unique_ptr<Backend>>& backends);
    void start();
    void stop();
private:
    std::vector<Backend *> backends_;

    std::thread worker_;

    std::atomic<bool> running_;

    bool checkBackend(Backend *backend);
};