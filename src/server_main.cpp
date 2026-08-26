#include <iostream>
#include <string>

#include "network/TcpServer.h"
#include "gateway/MessageParser.h"
#include "gateway/ExecutionReport.h"
#include "core/OrderBook.h"

int main()
{
    TcpServer server(8000);

    if (!server.start())
    {
        return 1;
    }

    std::cout << "Exchange Server started on port 8000\n";

    MessageParser parser;
    OrderBook orderBook;

    while (true)
    {
        std::cout << "\nWaiting for client...\n";

        if (!server.acceptClient())
        {
            continue;
        }

        std::cout << "Client connected.\n";

        std::string message;

        while (server.receiveMessage(message))
        {
            std::cout << "=========================================\n";
            std::cout << "Incoming Order : " << message << "\n";
            std::cout << "=========================================\n";

            auto order = parser.parse(message);

            if (!order)
            {
                ExecutionReport report(
                    ExecutionStatus::Rejected,
                    "Invalid Order"
                );

                server.sendMessage(report.toString());

                std::cout << report.toString() << "\n\n";

                continue;
            }

            orderBook.addOrder(*order);

            std::string response;

            response += "ACCEPTED OrderID=";
            response += std::to_string(order->getOrderId());
            response += "\n";

            const auto& newTrades = orderBook.getLastTrades();

            for (const auto& trade : newTrades)
            {
                response += "TRADE ";
                response += "Price=";
                response += std::to_string(trade.price);
                response += " ";
                response += "Qty=";
                response += std::to_string(trade.quantity);
                response += "\n";
            }

            if (newTrades.empty())
            {
                response += "RESTING";
            }
            else
            {
                response += "TRADE_EXECUTED";
            }

            server.sendMessage(response);

            std::cout << response << "\n\n";

            std::cout << "Current Trades\n";
            std::cout << "--------------\n";

            orderBook.printTrades();

            std::cout << "\nCurrent Order Book\n";
            std::cout << "------------------\n";

            orderBook.printOrderBook();

            std::cout << "\n";
        }

        std::cout << "Client disconnected.\n";
    }

    return 0;
}