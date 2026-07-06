#include "lb/RoundRobinScheduler.hpp"

RoundRobinScheduler::RoundRobinScheduler(std::vector<Backend>& backends)
    : backends_(backends),
      currentIndex_(0)
{
}

Backend* RoundRobinScheduler::selectBackend()
{
    Backend* backend = &backends_[currentIndex_];

    currentIndex_ =
        (currentIndex_ + 1) % backends_.size();

    return backend;
}