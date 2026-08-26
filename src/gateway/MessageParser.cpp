#include "gateway/MessageParser.h"
#include <sstream>
#include <vector>
#include <iostream>

MessageParser::MessageParser()
    : nextOrderId(1),
      timestamp(1)
{}

std::optional<Order> MessageParser::parse(const std::string& message)
{
    std::stringstream ss(message);

    std::vector<std::string> tokens;
    std::string token;

    while (ss >> token)
    {
        tokens.push_back(token);
    }

    if (tokens.size() < 3)
    {
        return std::nullopt;
    }

    OrderSide side;

    if (tokens[0] == "BUY")
    {
        side = OrderSide::Buy;
    }
    else if (tokens[0] == "SELL")
    {
        side = OrderSide::Sell;
    }
    else
    {
        return std::nullopt;
    }

    OrderType type;

    if (tokens[1] == "LIMIT")
    {
        type = OrderType::Limit;
    }
    else if (tokens[1] == "MARKET")
    {
        type = OrderType::Market;
    }
    else if (tokens[1] == "IOC")
    {
        type = OrderType::IOC;
    }
    else if (tokens[1] == "FOK")
    {
        type = OrderType::FOK;
    }
    else
    {
        return std::nullopt;
    }

    double price = 0;
    std::uint32_t quantity = 0;

    try
    {
        if (type == OrderType::Market)
        {
            if (tokens.size() != 3)
                return std::nullopt;

            quantity = std::stoul(tokens[2]);
        }
        else
        {
            if (tokens.size() != 4)
                return std::nullopt;

            price = std::stod(tokens[2]);
            quantity = std::stoul(tokens[3]);
        }
    }
    catch (...)
    {
        return std::nullopt;
    }

    Order order(
        nextOrderId++,
        side,
        type,
        price,
        quantity,
        timestamp++
    );

    return order;
}