cmake_minimum_required(VERSION 3.20)

project(LowLatencyOrderMatchingEngine VERSION 1.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)  
set(CMAKE_CXX_EXTENSIONS OFF)

include_directories(include)

add_executable(LowLatencyOrderMatchingEngine
    src/main.cpp
    src/Order.cpp
    src/OrderBook.cpp
)