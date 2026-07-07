#include "metrics/Metrics.hpp"

Metrics::Metrics()
{
    activeConnections_ = 0;
    totalConnections_ = 0;
    totalRequests_ = 0;
    bytesIn_ = 0;
    bytesOut_ = 0;
    healthyBackends_ = 0;
}

void Metrics::incrementActiveConnections()
{
    ++activeConnections_;
}

void Metrics::decrementActiveConnections()
{
    --activeConnections_;
}

void Metrics::incrementTotalConnections()
{
    ++totalConnections_;
}

void Metrics::incrementRequests()
{
    ++totalRequests_;
}

void Metrics::addBytesIn(uint64_t bytes)
{
    bytesIn_ += bytes;
}

void Metrics::addBytesOut(uint64_t bytes)
{
    bytesOut_ += bytes;
}

void Metrics::setHealthyBackends(int count)
{
    healthyBackends_ = count;
}

std::string Metrics::toJson() const
{
    return "{\n"
           "  \"active_connections\": " + std::to_string(activeConnections_.load()) + ",\n"
           "  \"total_connections\": " + std::to_string(totalConnections_.load()) + ",\n"
           "  \"requests\": " + std::to_string(totalRequests_.load()) + ",\n"
           "  \"bytes_in\": " + std::to_string(bytesIn_.load()) + ",\n"
           "  \"bytes_out\": " + std::to_string(bytesOut_.load()) + ",\n"
           "  \"healthy_backends\": " + std::to_string(healthyBackends_.load()) + "\n"
           "}";
}