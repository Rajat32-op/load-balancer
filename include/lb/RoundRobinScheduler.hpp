#pragma once

#include "lb/Scheduler.hpp"
#include "lb/Backend.hpp"

class RoundRobinScheduler : public Scheduler
{
public:
    explicit RoundRobinScheduler(std::vector<std::unique_ptr<Backend>>& backends);

    Backend* selectBackend() override;

private:
    std::vector<Backend *> backends_;
    size_t currentIndex_;
};