#pragma once
#include "lb/Backend.hpp"
#include <thread>
#include <atomic>
#include <vector>
#include <metrics/Metrics.hpp>

class HealthChecker
{
public:
    HealthChecker(std::vector<std::unique_ptr<Backend>>& backends,Metrics& metrics);
    void start();
    void stop();
private:
    std::vector<Backend *> backends_;

    std::thread worker_;

    std::atomic<bool> running_;

    Metrics& metrics_;

    bool checkBackend(Backend *backend);
};