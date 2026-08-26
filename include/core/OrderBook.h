#ifndef ORDERBOOK_H
#define ORDERBOOK_H

#include "Order.h"
#include "Trade.h"
#include <list>
#include <vector>
#include <map>
#include <unordered_map>
#include <functional>


struct OrderLocation { 
    double price;
    OrderSide side;
    std::list<Order>::iterator orderIt;
};

class OrderBook {
    private:
        std::map<double, std::list<Order>, std::greater<double>> buyOrders;
        std::map<double, std::list<Order>> sellOrders;
        std::unordered_map<std::uint64_t, OrderLocation> orderIndex;
        std::vector<Trade> trades;
        bool canFullyMatch(const Order& order) const;
        void matchOrder(Order& incomingOrder);
        std::vector<Trade> lastTrades;

    public:
        void addOrder(Order order);
        bool cancelOrder(std::uint64_t orderId);
        bool modifyOrder(std::uint64_t orderId, double newPrice, uint32_t newQuantity);
        std::size_t getTradeCount() const;
        
        const std::vector<Trade>& getLastTrades() const;

        void printTrades() const;
        void printBuyBook() const;
        void printSellBook() const;
        void printOrderBook() const;

};

#endif