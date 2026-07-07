#include "core/HealthChecker.hpp"

#include <unistd.h>

HealthChecker::HealthChecker(std::vector<std::unique_ptr<Backend>>& backends,Metrics& metrics)
    : running_(false),metrics_(metrics)
{
    for(auto& backend : backends)
    {
        backends_.push_back(backend.get());
    }
}

bool HealthChecker::checkBackend(Backend *backend)
{
    int fd = backend->connectBackend();
    if (fd != -1)
    {
        close(fd);
        return true;
    }
    return false;
}

void HealthChecker::start()
{
    running_.store(true);
    worker_ = std::thread([this]() {
        while (running_.load())
        {
            int countHealthy = 0;
            for (auto& backend : backends_)
            {
                bool isHealthy = checkBackend(backend);
                if (isHealthy)
                    countHealthy++;
                backend->setHealthy(isHealthy);
            }
            metrics_.setHealthyBackends(countHealthy);
            std::this_thread::sleep_for(std::chrono::seconds(5));
        }
    });
}

void HealthChecker::stop()
{
    running_.store(false);
    if (worker_.joinable())
    {
        worker_.join();
    }
}