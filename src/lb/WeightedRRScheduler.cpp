#include "lb/WeightedRRScheduler.hpp"

WeightedRoundRobinScheduler::WeightedRoundRobinScheduler(
    std::vector<std::unique_ptr<Backend>>& backends)
    : currentIndex_(0)
{
    for (auto& backend : backends)
    {
        for (int i = 0; i < backend->weight(); i++)
        {
            weightedBackends_.push_back(backend.get());
        }
    }
}

Backend* WeightedRoundRobinScheduler::selectBackend()
{
    if (weightedBackends_.empty())
        return nullptr;

    Backend* backend = weightedBackends_[currentIndex_];

    currentIndex_ =
        (currentIndex_ + 1) % weightedBackends_.size();

    return backend;
}