#include <chrono>
#include <cstdint>
#include <iostream>
#include <random>
#include <algorithm>
#include <string>
#include <thread>
#include <vector>
#include <numeric>

#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#endif

#ifdef _WIN32

bool sendAll(
    SOCKET socketHandle,
    const std::string& message)
{
    std::size_t totalSent = 0;

    while (totalSent < message.size())
    {
        const int bytesSent = send(
            socketHandle,
            message.data() + totalSent,
            static_cast<int>(
                message.size() - totalSent
            ),
            0
        );

        if (bytesSent <= 0)
        {
            return false;
        }

        totalSent +=
            static_cast<std::size_t>(bytesSent);
    }

    return true;
}

bool connectToServer(SOCKET& socketHandle)
{
    socketHandle =
        socket(
            AF_INET,
            SOCK_STREAM,
            IPPROTO_TCP
        );

    if (socketHandle == INVALID_SOCKET)
    {
        return false;
    }

    sockaddr_in serverAddress{};

    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(8000);

    inet_pton(
        AF_INET,
        "127.0.0.1",
        &serverAddress.sin_addr
    );

    if (connect(
            socketHandle,
            reinterpret_cast<sockaddr*>(
                &serverAddress
            ),
            sizeof(serverAddress)
        ) == SOCKET_ERROR)
    {
        closesocket(socketHandle);
        socketHandle = INVALID_SOCKET;
        return false;
    }

    return true;
}

int main()
{
    WSADATA wsaData;

    if (
        WSAStartup(
            MAKEWORD(2, 2),
            &wsaData
        ) != 0)
    {
        std::cerr
            << "WSAStartup failed.\n";

        return 1;
    }

    constexpr int RUNS = 5;
    constexpr std::uint64_t ORDER_COUNT = 100000;

    std::vector<double> throughputResults;

    std::cout
        << "====================================\n";
    std::cout
        << "Final Exchange Benchmark\n";
    std::cout
        << "====================================\n";
    std::cout
        << "Runs   : " << RUNS << "\n";
    std::cout
        << "Orders : " << ORDER_COUNT << "\n";
    std::cout
        << "Seed   : 42\n";
    std::cout
        << "====================================\n";

    for (int run = 1; run <= RUNS; ++run)
    {
        SOCKET socketHandle =
            INVALID_SOCKET;

        if (!connectToServer(socketHandle))
        {
            std::cerr
                << "Connection failed on run "
                << run
                << ".\n";

            WSACleanup();
            return 1;
        }

        std::uint64_t responsesReceived = 0;

        std::thread receiver(
            [&]()
            {
                char buffer[8192];
                std::string receiveBuffer;

                while (
                    responsesReceived <
                    ORDER_COUNT)
                {
                    const int bytesReceived =
                        recv(
                            socketHandle,
                            buffer,
                            sizeof(buffer),
                            0
                        );

                    if (bytesReceived <= 0)
                    {
                        break;
                    }

                    receiveBuffer.append(
                        buffer,
                        static_cast<std::size_t>(
                            bytesReceived
                        )
                    );

                    while (true)
                    {
                        const std::size_t newline =
                            receiveBuffer.find('\n');

                        if (
                            newline ==
                            std::string::npos)
                        {
                            break;
                        }

                        std::string line =
                            receiveBuffer.substr(
                                0,
                                newline
                            );

                        receiveBuffer.erase(
                            0,
                            newline + 1
                        );

                        if (
                            line.rfind(
                                "ACCEPTED OrderID=",
                                0
                            ) == 0)
                        {
                            ++responsesReceived;
                        }
                    }
                }
            }
        );

        std::mt19937 rng(
            42 + run - 1
        );

        std::uniform_int_distribution<int>
            sideDistribution(0, 1);

        std::uniform_int_distribution<int>
            priceDistribution(9900, 10100);

        std::uniform_int_distribution<int>
            quantityDistribution(1, 100);

        const auto start =
            std::chrono::steady_clock::now();

        for (
            std::uint64_t i = 0;
            i < ORDER_COUNT;
            ++i)
        {
            const bool buy =
                sideDistribution(rng) == 0;

            const int price =
                priceDistribution(rng);

            const int quantity =
                quantityDistribution(rng);

            std::string message =
                buy
                    ? "BUY LIMIT "
                    : "SELL LIMIT ";

            message +=
                std::to_string(
                    static_cast<double>(
                        price
                    ) / 100.0
                );

            message += " ";

            message +=
                std::to_string(quantity);

            message += "\n";

            if (
                !sendAll(
                    socketHandle,
                    message
                ))
            {
                std::cerr
                    << "Send failed on run "
                    << run
                    << " at order "
                    << i + 1
                    << ".\n";

                closesocket(socketHandle);

                if (receiver.joinable())
                {
                    receiver.detach();
                }

                WSACleanup();
                return 1;
            }
        }

        const auto sendEnd =
            std::chrono::steady_clock::now();

        while (
            responsesReceived <
            ORDER_COUNT)
        {
            std::this_thread::sleep_for(
                std::chrono::milliseconds(1)
            );
        }

        const auto end =
            std::chrono::steady_clock::now();

        if (receiver.joinable())
        {
            receiver.join();
        }

        shutdown(
            socketHandle,
            SD_BOTH
        );

        closesocket(socketHandle);

        const double elapsed =
            std::chrono::duration<double>(
                end - start
            ).count();

        const double throughput =
            static_cast<double>(
                ORDER_COUNT
            ) / elapsed;

        const double sendTime =
            std::chrono::duration<double>(
                sendEnd - start
            ).count();

        std::cout
            << "\nRun "
            << run
            << " / "
            << RUNS
            << "\n";

        std::cout
            << "Orders     : "
            << ORDER_COUNT
            << "\n";

        std::cout
            << "Responses  : "
            << responsesReceived
            << "\n";

        std::cout
            << "Send Time  : "
            << sendTime
            << " sec\n";

        std::cout
            << "Total Time : "
            << elapsed
            << " sec\n";

        std::cout
            << "Throughput : "
            << throughput
            << " orders/sec\n";

        throughputResults.push_back(
            throughput
        );
    }

    const double average =
        std::accumulate(
            throughputResults.begin(),
            throughputResults.end(),
            0.0
        ) / throughputResults.size();

    const double minimum =
        *std::min_element(
            throughputResults.begin(),
            throughputResults.end()
        );

    const double maximum =
        *std::max_element(
            throughputResults.begin(),
            throughputResults.end()
        );

    std::cout
        << "\n====================================\n";

    std::cout
        << "FINAL BENCHMARK RESULT\n";

    std::cout
        << "====================================\n";

    for (
        std::size_t i = 0;
        i < throughputResults.size();
        ++i)
    {
        std::cout
            << "Run "
            << i + 1
            << " : "
            << throughputResults[i]
            << " orders/sec\n";
    }

    std::cout
        << "------------------------------------\n";

    std::cout
        << "Average : "
        << average
        << " orders/sec\n";

    std::cout
        << "Minimum : "
        << minimum
        << " orders/sec\n";

    std::cout
        << "Maximum : "
        << maximum
        << " orders/sec\n";

    std::cout
        << "====================================\n";

    WSACleanup();

    return 0;
}

#else

int main()
{
    std::cerr
        << "LoadTest is currently implemented "
        << "for Windows.\n";

    return 1;
}

#endif