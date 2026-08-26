#ifndef ORDER_H
#define ORDER_H

#include <cstdint>

enum class OrderSide {
    Buy,
    Sell
};

enum class OrderType {
    Limit,
    Market,
    IOC,
    FOK
};

class Order {
    private:
        std::uint64_t orderId;
        OrderSide side;
        OrderType type;
        double price;
        std::uint32_t quantity;
        std::uint64_t timestamp;
    
    public:
        Order(
            std::uint64_t orderId, 
            OrderSide side, 
            OrderType type, 
            double price, 
            std::uint32_t quantity, 
            std:: uint64_t timestamp
        );
        
        void display() const;
        
        std::uint64_t getOrderId() const;
        OrderSide getSide() const;
        OrderType getType() const;
        double getPrice() const;
        std::uint32_t getQuantity() const;
        std::uint64_t getTimestamp() const;

        void reduceQuantity(std::uint32_t amount);

        void setPrice(double newPrice);
        void setQuantity(std::uint32_t newQuantity);
        void setTimestamp(std::uint64_t newTimestamp);
        
};
    
#endif