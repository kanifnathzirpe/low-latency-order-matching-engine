#include "engine/MatchingEngine.h"

void MatchingEngine::processOrder(const Order& order)
{
    orderBook.addOrder(order);
}

const std::vector<Trade>& MatchingEngine::getLastTrades() const
{
    return orderBook.getLastTrades();
}

void MatchingEngine::printTrades() const
{
    orderBook.printTrades();
}

void MatchingEngine::printOrderBook() const
{
    orderBook.printOrderBook();
}