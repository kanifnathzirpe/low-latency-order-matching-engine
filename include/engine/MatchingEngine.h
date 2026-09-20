#ifndef MATCHING_ENGINE_H
#define MATCHING_ENGINE_H

#include "core/OrderBook.h"
#include "core/Order.h"

class MatchingEngine
{
private:
    OrderBook orderBook;

public:
    void processOrder(const Order& order);

    const std::vector<Trade>& getLastTrades() const;

    void printTrades() const;
    void printOrderBook() const;
};

#endif