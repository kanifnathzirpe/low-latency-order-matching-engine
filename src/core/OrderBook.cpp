#include "core/OrderBook.h"
#include "core/Trade.h"
#include <iostream>
#include <algorithm>

void OrderBook::addOrder(Order order) {
    lastTrades.clear();
    if(order.getType()==OrderType::FOK && !canFullyMatch(order)) return;
    matchOrder(order);
    if(order.getQuantity()==0) return;
    if(order.getType()==OrderType::IOC || 
        order.getType()==OrderType::Market || 
        order.getType()==OrderType::FOK) {
            return;
    }
    const auto price = order.getPrice();
    
    if(order.getSide()==OrderSide::Buy) {
        auto& orders = buyOrders[price];
        orders.push_back(order);
        auto orderIt = std::prev(orders.end());
        orderIndex[order.getOrderId()] = {price, OrderSide::Buy, orderIt};
    } else {
        auto& orders = sellOrders[price];
        orders.push_back(order);
        auto orderIt = std::prev(orders.end());
        orderIndex[order.getOrderId()] = {price, OrderSide::Sell, orderIt};
    }
}

bool OrderBook::cancelOrder(std::uint64_t orderId) {
    auto indexIt = orderIndex.find(orderId);

    if(indexIt==orderIndex.end()) return false;
    
    const auto price = indexIt->second.price;
    const auto side = indexIt->second.side;
    auto orderIt = indexIt->second.orderIt;

    if(side==OrderSide::Buy) {
        auto priceIt = buyOrders.find(price);
        if(priceIt!=buyOrders.end()) {
            priceIt->second.erase(orderIt);
            if(priceIt->second.empty()) {
                buyOrders.erase(priceIt);
            }
            
        }
    }
    else {
        auto priceIt = sellOrders.find(price);
        if(priceIt!=sellOrders.end()) {
            priceIt->second.erase(orderIt);
            if(priceIt->second.empty()) {
                sellOrders.erase(priceIt);
            }
        }
    }
    orderIndex.erase(indexIt);
    return true;
}

bool OrderBook::modifyOrder(std::uint64_t orderId, double newPrice, std::uint32_t newQuantity) {
    if(newPrice <= 0 || newQuantity==0) return false;
    auto indexIt = orderIndex.find(orderId);
    if(indexIt==orderIndex.end()) return false;
    
    const auto& entry = indexIt->second;
    Order modifiedOrder = *(entry.orderIt);

    cancelOrder(orderId);

    modifiedOrder.setPrice(newPrice);
    modifiedOrder.setQuantity(newQuantity);
    modifiedOrder.setTimestamp(modifiedOrder.getTimestamp()+1);

    addOrder(modifiedOrder);
    return true;
} 

bool OrderBook::canFullyMatch(const Order& order) const {
    std::uint32_t remaining = order.getQuantity();
    if(order.getSide()==OrderSide::Buy) {
        for(const auto& [price,orders] : sellOrders) {
            if((order.getType() == OrderType::Limit ||
                order.getType() == OrderType::IOC ||
                order.getType() == OrderType::FOK) && price > order.getPrice()) {
                    break;
            }
            for(const auto& restingOrder : orders) {
                if(remaining <= restingOrder.getQuantity()) return true;
                remaining -= restingOrder.getQuantity();
            }
        }
    } else {
        for(const auto& [price, orders] : buyOrders) {
            if((order.getType() == OrderType::Limit ||
                order.getType() == OrderType::IOC ||
                order.getType() == OrderType::FOK) && price < order.getPrice()) {
                    break;
            }
            for(const auto& restingOrder : orders) {
                if(remaining <= restingOrder.getQuantity()) return true;
                remaining -= restingOrder.getQuantity();
            }
        }
    }
    return false;
}

void OrderBook::matchOrder(Order& incomingOrder) {
    if(incomingOrder.getSide()==OrderSide::Buy) {
        while(incomingOrder.getQuantity()>0 && !sellOrders.empty()) {
            auto bestSell = sellOrders.begin();
            const double sellPrice = bestSell->first;

            if((incomingOrder.getType()==OrderType::Limit || incomingOrder.getType()==OrderType::IOC) && sellPrice > incomingOrder.getPrice()) {
                break;
            }
            
            auto& orders = bestSell->second;
            Order& restingOrder = orders.front();

            const std::uint32_t tradeQuantity = std::min(incomingOrder.getQuantity(),restingOrder.getQuantity());
            Trade trade {
                incomingOrder.getOrderId(),
                restingOrder.getOrderId(),
                restingOrder.getPrice(),
                tradeQuantity
            };
            trades.push_back(trade);
            lastTrades.push_back(trade);
            restingOrder.reduceQuantity(tradeQuantity);
            incomingOrder.reduceQuantity(tradeQuantity);
            
            if(restingOrder.getQuantity()==0) {
                orderIndex.erase(restingOrder.getOrderId());
                orders.pop_front();

                if(orders.empty()) {
                    sellOrders.erase(bestSell);
                }
            }
        }
    } else {
        while(incomingOrder.getQuantity()>0 && !buyOrders.empty()) {
            auto bestBuy = buyOrders.begin();
            const double buyPrice = bestBuy->first;
            
            auto& orders = bestBuy->second;
            Order& restingOrder = orders.front();
            
            if((incomingOrder.getType()==OrderType::Limit || incomingOrder.getType()==OrderType::IOC) && buyPrice < incomingOrder.getPrice()) {
                break;
            }
            
            const std::uint32_t tradeQuantity = std::min(incomingOrder.getQuantity(),restingOrder.getQuantity());
            Trade trade {
                restingOrder.getOrderId(),
                incomingOrder.getOrderId(),
                restingOrder.getPrice(),
                tradeQuantity
            };
            trades.push_back(trade);
            lastTrades.push_back(trade);
            restingOrder.reduceQuantity(tradeQuantity);
            incomingOrder.reduceQuantity(tradeQuantity);
            
            if(restingOrder.getQuantity()==0) {
                orderIndex.erase(restingOrder.getOrderId());
                orders.pop_front();
    
                if(orders.empty()) {
                    buyOrders.erase(bestBuy);
                }
            }            
        }
    }
}

std::size_t OrderBook::getTradeCount() const {
    return trades.size();
}

void OrderBook::printTrades() const {
    std::cout << "\n========== TRADE HISTORY ==========\n";

    if (trades.empty()) {
        std::cout << "No trades executed.\n";
        return;
    }

    int tradeNumber = 1;

    for (const Trade& trade : trades) {
        std::cout << "\nTrade #" << tradeNumber++ << '\n';
        std::cout << "-----------------------------------\n";
        std::cout << "Buy Order ID : " << trade.buyOrderId << '\n';
        std::cout << "Sell Order ID: " << trade.sellOrderId << '\n';
        std::cout << "Trade Price  : " << trade.price << '\n';
        std::cout << "Quantity     : " << trade.quantity << '\n';
        std::cout << "-----------------------------------\n";
    }
}

void OrderBook::printBuyBook() const {
    std::cout << "----- Buy Book -----" << std::endl;
    for(const auto& [price,orders] : buyOrders) {
        std::cout << "Price: " << price << std::endl;
        for(const auto& order : orders) {
            order.display();
        }
    }
}

void OrderBook::printSellBook() const {
    std::cout << "----- Sell Book -----" << std::endl;
    for(const auto& [price,orders] : sellOrders) {
        std::cout << "Price: " << price << std::endl;
        for(const auto& order : orders) {
            order.display();
        }
    }
}

void OrderBook::printOrderBook() const {
    printBuyBook();
    printSellBook();
}

const std::vector<Trade>& OrderBook::getLastTrades() const {
    return lastTrades;
}