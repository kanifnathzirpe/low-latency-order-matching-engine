#ifndef LATENCY_METRICS_H
#define LATENCY_METRICS_H

#include <cstdint>
#include <vector>
#include <mutex>

class LatencyMetrics
{
private:
    std::vector<std::uint64_t> samples;
    mutable std::mutex mutex;

public:
    void record(std::uint64_t nanoseconds);
    void print() const;
    void clear();
    std::size_t size() const;
};

#endif