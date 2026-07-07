#pragma once

#include <atomic>
#include <string>

class Metrics
{
public:
    Metrics();

    void incrementActiveConnections();
    void decrementActiveConnections();

    void incrementTotalConnections();
    void incrementRequests();

    void addBytesIn(uint64_t bytes);
    void addBytesOut(uint64_t bytes);

    void setHealthyBackends(int count);

    std::string toJson() const;

private:
    std::atomic<int> activeConnections_;

    std::atomic<uint64_t> totalConnections_;
    std::atomic<uint64_t> totalRequests_;

    std::atomic<uint64_t> bytesIn_;
    std::atomic<uint64_t> bytesOut_;

    std::atomic<int> healthyBackends_;
};