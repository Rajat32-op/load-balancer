#pragma once

#include "lb/Scheduler.hpp"
#include "lb/Backend.hpp"

#include <vector>

class WeightedRoundRobinScheduler : public Scheduler
{
public:
    explicit WeightedRoundRobinScheduler(std::vector<Backend>& backends);

    Backend* selectBackend() override;

private:
    std::vector<Backend*> weightedBackends_;
    size_t currentIndex_;
};