#include "Order.h"
#include "OrderBook.h"
#include <iostream>
#include <chrono>
#include <algorithm>
#include <random>
#include <vector>
#include <iomanip>

int main() {

    OrderBook book;

    std::cout << "\n=========================================\n";
    std::cout << " LOW-LATENCY ORDER MATCHING ENGINE DEMO\n";
    std::cout << "=========================================\n";

    // =====================================================
    // 1. LIMIT ORDERS
    // =====================================================

    std::cout << "\n========== LIMIT ORDERS ==========\n";

    book.addOrder(Order(1, OrderSide::Sell, OrderType::Limit, 101, 100, 1));
    book.addOrder(Order(2, OrderSide::Sell, OrderType::Limit, 102, 50, 2));
    book.addOrder(Order(3, OrderSide::Buy, OrderType::Limit, 99, 80, 3));

    book.printOrderBook();

    // =====================================================
    // 2. MARKET ORDER
    // =====================================================

    std::cout << "\n========== MARKET BUY ==========\n";

    book.addOrder(Order(4, OrderSide::Buy, OrderType::Market, 0, 120, 4));

    book.printOrderBook();

    // =====================================================
    // 3. PARTIAL MATCH
    // =====================================================

    std::cout << "\n========== PARTIAL MATCH ==========\n";

    book.addOrder(Order(5, OrderSide::Sell, OrderType::Limit, 100, 100, 5));
    book.addOrder(Order(6, OrderSide::Buy, OrderType::Limit, 100, 40, 6));

    book.printOrderBook();

    // =====================================================
    // 4. ORDER MODIFICATION
    // =====================================================

    std::cout << "\n========== ORDER MODIFICATION ==========\n";

    book.modifyOrder(3, 101, 80);

    book.printOrderBook();

    // =====================================================
    // 5. ORDER CANCELLATION
    // =====================================================

    std::cout << "\n========== ORDER CANCELLATION ==========\n";

    book.cancelOrder(2);

    book.printOrderBook();

    // =====================================================
    // 6. IOC ORDER
    // =====================================================

    std::cout << "\n========== IOC ORDER ==========\n";

    book.addOrder(Order(7, OrderSide::Buy, OrderType::IOC, 101, 150, 7));

    book.printOrderBook();

    // =====================================================
    // 7. FOK SUCCESS
    // =====================================================

    std::cout << "\n========== FOK SUCCESS ==========\n";

    book.addOrder(Order(8, OrderSide::Sell, OrderType::Limit, 99, 100, 8));
    book.addOrder(Order(9, OrderSide::Buy, OrderType::FOK, 100, 100, 9));

    book.printOrderBook();

    // =====================================================
    // 8. FOK FAILURE
    // =====================================================

    std::cout << "\n========== FOK FAILURE ==========\n";

    book.addOrder(Order(10, OrderSide::Sell, OrderType::Limit, 105, 20, 10));
    book.addOrder(Order(11, OrderSide::Buy, OrderType::FOK, 105, 100, 11));

    book.printOrderBook();

    // =====================================================
    // TRADE HISTORY
    // =====================================================

    std::cout << "\n========== FINAL ORDER BOOK ==========\n";

    book.printOrderBook();

    std::cout << "\n========== TRADE HISTORY ==========\n";

    book.printTrades();

    return 0;
}