#ifndef MESSAGEPARSER_H
#define MESSAGEPARSER_H

#include <string>
#include <optional>
#include <atomic>

#include "core/Order.h"

class MessageParser
{
private:
    std::atomic<std::uint64_t> nextOrderId;
    std::atomic<std::uint64_t> timestamp;

public:
    MessageParser();

    std::optional<Order> parse(const std::string& message);
};

#endif