cmake_minimum_required(VERSION 3.16)

project(LowLatencyOrderMatchingEngine)

set(CMAKE_CXX_STANDARD 17)

include_directories(include)

set(COMMON_SOURCES
    src/core/Order.cpp
    src/core/OrderBook.cpp
    src/gateway/MessageParser.cpp
    src/gateway/ExecutionReport.cpp
    src/network/TcpServer.cpp
    src/network/TcpClient.cpp
)

add_executable(ExchangeServer
    src/server_main.cpp
    ${COMMON_SOURCES}
)

add_executable(ExchangeClient
    src/client_main.cpp
    ${COMMON_SOURCES}
)