#include "lb/LeastConnectionScheduler.hpp"

LeastConnectionScheduler::LeastConnectionScheduler(std::vector<Backend>& backends)
    : backends_(backends)
{
}

Backend * LeastConnectionScheduler::selectBackend()
{
    Backend* best = nullptr;

    for(auto& backend : backends_)
    {
        if(!backend.healthy())
            continue;

        if(best == nullptr ||
        backend.activeConnections() <
        best->activeConnections())
        {
            best = &backend;
        }
    }

    return best;
}