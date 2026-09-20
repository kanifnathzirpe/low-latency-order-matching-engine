#ifndef ORDER_REQUEST_H
#define ORDER_REQUEST_H

#include "core/Order.h"
#include "network/SocketTypes.h"

struct OrderRequest
{
    Order order;
    SocketHandle clientSocket;
};

#endif