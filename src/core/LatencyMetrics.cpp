#include "core/LatencyMetrics.h"

#include <algorithm>
#include <iostream>
#include <numeric>

void LatencyMetrics::record(std::uint64_t nanoseconds)
{
    std::lock_guard<std::mutex> lock(mutex);
    samples.push_back(nanoseconds);
}

std::size_t LatencyMetrics::size() const
{
    std::lock_guard<std::mutex> lock(mutex);
    return samples.size();
}

void LatencyMetrics::clear()
{
    std::lock_guard<std::mutex> lock(mutex);
    samples.clear();
}

void LatencyMetrics::print() const
{
    std::lock_guard<std::mutex> lock(mutex);

    if (samples.empty())
    {
        std::cout << "No latency samples.\n";
        return;
    }

    std::vector<std::uint64_t> sorted = samples;

    std::sort(
        sorted.begin(),
        sorted.end()
    );

    const auto percentile =
        [&](double p)
        {
            std::size_t index =
                static_cast<std::size_t>(
                    p * (sorted.size() - 1)
                );

            return sorted[index];
        };

    const auto total =
        std::accumulate(
            sorted.begin(),
            sorted.end(),
            std::uint64_t{0}
        );

    const double average =
        static_cast<double>(total) /
        sorted.size();

    std::cout << "\n========== LATENCY ==========\n";
    std::cout << "Samples : " << sorted.size() << "\n";
    std::cout << "Min     : " << sorted.front() << " ns\n";
    std::cout << "Average : " << average << " ns\n";
    std::cout << "P50     : " << percentile(0.50) << " ns\n";
    std::cout << "P95     : " << percentile(0.95) << " ns\n";
    std::cout << "P99     : " << percentile(0.99) << " ns\n";
    std::cout << "Max     : " << sorted.back() << " ns\n";
}