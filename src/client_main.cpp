#include <iostream>
#include <string>

#include "network/TcpClient.h"

int main()
{
    TcpClient client("127.0.0.1", 8000);

    if (!client.connectToServer())
    {
        std::cerr << "Failed to connect to server.\n";
        return 1;
    }

    std::cout << "=========================================\n";
    std::cout << " Connected to Exchange Server\n";
    std::cout << "=========================================\n\n";

    std::cout << "Supported Orders:\n";
    std::cout << "  BUY LIMIT <price> <qty>\n";
    std::cout << "  SELL LIMIT <price> <qty>\n";
    std::cout << "  BUY MARKET <qty>\n";
    std::cout << "  SELL MARKET <qty>\n";
    std::cout << "  BUY IOC <price> <qty>\n";
    std::cout << "  SELL IOC <price> <qty>\n";
    std::cout << "  BUY FOK <price> <qty>\n";
    std::cout << "  SELL FOK <price> <qty>\n\n";

    std::cout << "Type 'exit' to quit.\n\n";

    std::string order;

    while (true)
    {
        std::cout << ">> ";

        std::getline(std::cin, order);

        if (order == "exit")
        {
            break;
        }

        if (order.empty())
        {
            continue;
        }

        if (!client.sendMessage(order))
        {
            std::cerr << "\nConnection lost while sending order.\n";
            break;
        }

        std::string response;

        if (!client.receiveMessage(response))
        {
            std::cerr << "\nServer disconnected.\n";
            break;
        }

        std::cout << "\n========== Execution Report ==========\n";
        std::cout << response << '\n';
        std::cout << "======================================\n\n";
    }

    client.disconnect();

    std::cout << "\nDisconnected from Exchange Server.\n";

    return 0;
}