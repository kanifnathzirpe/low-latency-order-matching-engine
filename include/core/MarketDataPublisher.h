#ifndef MARKET_DATA_PUBLISHER_H
#define MARKET_DATA_PUBLISHER_H

#include "core/ConcurrentQueue.h"
#include "core/MarketDataEvent.h"

class MarketDataPublisher
{
private:
    ConcurrentQueue<MarketDataEvent>& queue;

public:
    explicit MarketDataPublisher(
        ConcurrentQueue<MarketDataEvent>& queue
    );

    void publish(const MarketDataEvent& event);
};

#endif