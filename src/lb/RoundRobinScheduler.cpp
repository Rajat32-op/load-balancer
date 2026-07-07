#include "lb/RoundRobinScheduler.hpp"

RoundRobinScheduler::RoundRobinScheduler(
    std::vector<std::unique_ptr<Backend>>& backends)
    : currentIndex_(0)
{
    backends_.reserve(backends.size());

    for (auto& backend : backends)
    {
        backends_.push_back(backend.get());
    }
}

Backend* RoundRobinScheduler::selectBackend()
{
    Backend* backend = backends_[currentIndex_];

    currentIndex_ =
        (currentIndex_ + 1) % backends_.size();

    return backend;
}