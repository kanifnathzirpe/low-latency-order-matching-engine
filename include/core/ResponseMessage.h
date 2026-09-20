#ifndef RESPONSE_MESSAGE_H
#define RESPONSE_MESSAGE_H

#include <string>
#include "network/SocketTypes.h"

struct ResponseMessage
{
    SocketHandle clientSocket;
    std::string message;
};

#endif