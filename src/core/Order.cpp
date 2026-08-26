#include <iostream>
#include "core/Order.h"

 Order::Order(std::uint64_t orderId, OrderSide side, OrderType type, double price, std::uint32_t quantity, std:: uint64_t timestamp)
    : orderId(orderId), side(side), type(type), price(price), quantity(quantity), timestamp(timestamp) 
    {}

void Order::display() const {
    std::string orderType;

    if (type == OrderType::Limit)
        orderType = "Limit";
    else if (type == OrderType::Market)
        orderType = "Market";
    else if(type==OrderType::IOC)
        orderType = "IOC";
    else 
        orderType = "FOK";
        
    std::cout<<"Order ID: " << orderId
            <<" | Side: " << (side==OrderSide::Buy? "Buy":"Sell")
            <<" | Type: " << orderType
            <<" | Price: " << price
            <<" | Quantity: " << quantity
            <<" | Timestamp: " << timestamp
            << std::endl;
}

std::uint64_t Order::getOrderId() const {
    return orderId;
}

OrderSide Order::getSide() const {
    return side;
}

OrderType Order::getType() const {
    return type;
}

double Order::getPrice() const {
    return price;
}

std::uint32_t Order::getQuantity() const {
    return quantity;
}

std::uint64_t Order::getTimestamp() const {
    return timestamp;
}

void Order::reduceQuantity(std::uint32_t amount) {
    if(amount >= quantity) {
        quantity = 0;
    } else {
        quantity -= amount;
    }
}

void Order::setPrice(double newPrice) {
    price = newPrice;
}

void Order::setQuantity(std::uint32_t newQuantity) {
    quantity = newQuantity;
}

void Order::setTimestamp(std::uint64_t newTimestamp) {
    timestamp = newTimestamp;
}