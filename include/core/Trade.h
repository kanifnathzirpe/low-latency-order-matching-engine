#ifndef TRADE_H
#define TRADE_H

#include <cstdint>

struct Trade {
    std::uint64_t buyOrderId;
    std::uint64_t sellOrderId;
    double price;
    std::uint32_t quantity;
};


#endif