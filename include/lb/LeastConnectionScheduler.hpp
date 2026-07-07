#pragma once
#include "lb/Scheduler.hpp"
#include "lb/Backend.hpp"

class LeastConnectionScheduler : public Scheduler
{
public:
    explicit LeastConnectionScheduler(std::vector<std::unique_ptr<Backend>>& backends);

    Backend* selectBackend() override;
private:
    std::vector<Backend *> backends_;
};
