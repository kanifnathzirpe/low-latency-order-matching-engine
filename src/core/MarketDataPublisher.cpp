#include "core/MarketDataPublisher.h"

MarketDataPublisher::MarketDataPublisher(
    ConcurrentQueue<MarketDataEvent>& queue)
    : queue(queue)
{
}

void MarketDataPublisher::publish(
    const MarketDataEvent& event)
{
    queue.push(event);
}