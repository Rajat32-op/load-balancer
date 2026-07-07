#include "core/HealthChecker.hpp"
#include <unistd.h>

HealthChecker::HealthChecker(std::vector<std::unique_ptr<Backend>>& backends)
    : running_(false)
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
            for (auto& backend : backends_)
            {
                bool isHealthy = checkBackend(backend);
                backend->setHealthy(isHealthy);
            }
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