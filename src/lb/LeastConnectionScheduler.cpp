#include "lb/LeastConnectionScheduler.hpp"
#include <memory>

LeastConnectionScheduler::LeastConnectionScheduler(std::vector<std::unique_ptr<Backend>>& backends)
{
    backends_.reserve(backends.size());

    for (auto& backend : backends)
    {
        backends_.push_back(backend.get());
    }
}

Backend * LeastConnectionScheduler::selectBackend()
{
    Backend* best = nullptr;

    for(auto& backend : backends_)
    {
        if(!backend->healthy())
            continue;

        if(best == nullptr ||
        backend->activeConnections() <
        best->activeConnections())
        {
            best = backend;
        }
    }

    return best;
}