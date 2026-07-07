#pragma once

#include "metrics/Metrics.hpp"

#include <atomic>
#include <thread>

class MetricsServer
{
public:
    explicit MetricsServer(Metrics& metrics);

    void start();
    void stop();

private:
    void run();

    Metrics& metrics_;

    std::thread worker_;
    std::atomic<bool> running_;
};