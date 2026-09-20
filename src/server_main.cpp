#include <iostream>
#include <string>
#include <thread>
#include <mutex>
#include <chrono>
#include <cstdint>

#include "network/TcpServer.h"
#include "gateway/MessageParser.h"
#include "gateway/ExecutionReport.h"
#include "engine/MatchingEngine.h"
#include "core/OrderRequest.h"
#include "core/ConcurrentQueue.h"
#include "core/ResponseMessage.h"
#include "core/MarketDataEvent.h"
#include "core/MarketDataPublisher.h"
#include "core/LatencyMetrics.h"

void handleClient(
    TcpServer& server,
    SocketHandle clientSocket,
    MessageParser& parser,
    ConcurrentQueue<OrderRequest>& orderQueue,
    ConcurrentQueue<ResponseMessage>& responseQueue,
    std::mutex& outputMutex)
{
    {
        std::lock_guard<std::mutex> lock(outputMutex);
        std::cout << "Client connected.\n";
    }

    std::string message;
    std::string receiveBuffer;

    while (
        server.receiveMessage(
            clientSocket,
            message,
            receiveBuffer))
    {
        auto order = parser.parse(message);

        if (!order)
        {
            ExecutionReport report(
                ExecutionStatus::Rejected,
                "Invalid Order"
            );

            responseQueue.push(
                ResponseMessage{
                    clientSocket,
                    report.toString()
                }
            );

            continue;
        }

        OrderRequest request{
            *order,
            clientSocket
        };

        orderQueue.push(std::move(request));
    }

    {
        std::lock_guard<std::mutex> lock(outputMutex);
        std::cout << "Client disconnected.\n";
    }
}

int main()
{
    TcpServer server(8000);

    if (!server.start())
    {
        return 1;
    }

    std::cout
        << "Exchange Server started on port 8000\n";

    MessageParser parser;
    MatchingEngine matchingEngine;

    ConcurrentQueue<OrderRequest> orderQueue;
    ConcurrentQueue<ResponseMessage> responseQueue;
    ConcurrentQueue<MarketDataEvent> marketDataQueue;

    LatencyMetrics latencyMetrics;

    MarketDataPublisher marketDataPublisher(
        marketDataQueue
    );

    std::mutex outputMutex;

    std::thread matchingThread(
        [&]()
        {
            while (true)
            {
                auto request =
                    orderQueue.waitAndPop();

                if (!request.has_value())
                {
                    break;
                }

                const auto start =
                    std::chrono::steady_clock::now();

                const Order& order =
                    request->order;

                const SocketHandle clientSocket =
                    request->clientSocket;

                matchingEngine.processOrder(order);

                std::string response;

                response += "ACCEPTED OrderID=";

                response +=
                    std::to_string(
                        order.getOrderId()
                    );

                response += "\n";

                const auto& newTrades =
                    matchingEngine.getLastTrades();

                for (const auto& trade : newTrades)
                {
                    std::string marketDataMessage;

                    marketDataMessage += "TRADE ";
                    marketDataMessage += "BuyOrderID=";

                    marketDataMessage +=
                        std::to_string(
                            trade.buyOrderId
                        );

                    marketDataMessage += " ";
                    marketDataMessage += "SellOrderID=";

                    marketDataMessage +=
                        std::to_string(
                            trade.sellOrderId
                        );

                    marketDataMessage += " ";
                    marketDataMessage += "Price=";

                    marketDataMessage +=
                        std::to_string(
                            trade.price
                        );

                    marketDataMessage += " ";
                    marketDataMessage += "Qty=";

                    marketDataMessage +=
                        std::to_string(
                            trade.quantity
                        );

                    marketDataPublisher.publish(
                        MarketDataEvent{
                            marketDataMessage
                        }
                    );

                    response += "TRADE ";
                    response += "Price=";

                    response +=
                        std::to_string(
                            trade.price
                        );

                    response += " ";
                    response += "Qty=";

                    response +=
                        std::to_string(
                            trade.quantity
                        );

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

                responseQueue.push(
                    ResponseMessage{
                        clientSocket,
                        response
                    }
                );

                const auto end =
                    std::chrono::steady_clock::now();

                const auto latency =
                    std::chrono::duration_cast<
                        std::chrono::nanoseconds
                    >(end - start).count();

                latencyMetrics.record(
                    static_cast<std::uint64_t>(
                        latency
                    )
                );
                if (latencyMetrics.size() == 100000)
                {
                    latencyMetrics.print();
                    latencyMetrics.clear();
                }
            }
        }
    );

    std::thread responseThread(
        [&]()
        {
            while (true)
            {
                auto response =
                    responseQueue.waitAndPop();

                if (!response.has_value())
                {
                    break;
                }

                if (!server.sendMessage(
                        response->clientSocket,
                        response->message))
                {
                    std::lock_guard<std::mutex> lock(
                        outputMutex
                    );

                    std::cout
                        << "Failed to send response "
                        << "to client.\n";
                }
            }
        }
    );

    std::thread marketDataThread(
        [&]()
        {
            while (true)
            {
                auto event =
                    marketDataQueue.waitAndPop();

                if (!event.has_value())
                {
                    break;
                }
            }
        }
    );

    while (true)
    {
        SocketHandle clientSocket =
            server.acceptClient();

        if (clientSocket == InvalidSocket)
        {
            continue;
        }

        std::thread(
            handleClient,
            std::ref(server),
            clientSocket,
            std::ref(parser),
            std::ref(orderQueue),
            std::ref(responseQueue),
            std::ref(outputMutex)
        ).detach();
    }

    orderQueue.close();

    if (matchingThread.joinable())
    {
        matchingThread.join();
    }

    responseQueue.close();

    if (responseThread.joinable())
    {
        responseThread.join();
    }

    marketDataQueue.close();

    if (marketDataThread.joinable())
    {
        marketDataThread.join();
    }

    server.stop();

    return 0;
}