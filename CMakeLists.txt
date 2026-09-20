cmake_minimum_required(VERSION 3.16)

project(LowLatencyOrderMatchingEngine)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(include)

set(COMMON_SOURCES
    src/core/Order.cpp
    src/core/OrderBook.cpp
    src/gateway/MessageParser.cpp
    src/gateway/ExecutionReport.cpp
    src/network/TcpServer.cpp
    src/network/TcpClient.cpp
    src/engine/MatchingEngine.cpp
    src/core/MarketDataPublisher.cpp
    src/core/LatencyMetrics.cpp
)

add_executable(ExchangeServer
    src/server_main.cpp
    ${COMMON_SOURCES}
)

add_executable(ExchangeClient
    src/client_main.cpp
    ${COMMON_SOURCES}
)

add_executable(LoadTest
    tests/LoadTest.cpp
)

if(WIN32)
    target_link_libraries(ExchangeServer PRIVATE ws2_32)
    target_link_libraries(ExchangeClient PRIVATE ws2_32)
    target_link_libraries(LoadTest PRIVATE ws2_32)
endif()