#ifndef SOCKET_TYPES_H
#define SOCKET_TYPES_H

#ifdef _WIN32
#include <winsock2.h>

using SocketHandle = SOCKET;
constexpr SocketHandle InvalidSocket = INVALID_SOCKET;

inline bool initializeSockets()
{
    WSADATA data{};
    return WSAStartup(MAKEWORD(2, 2), &data) == 0;
}
#else
#include <sys/types.h>

using SocketHandle = int;
constexpr SocketHandle InvalidSocket = -1;

inline bool initializeSockets()
{
    return true;
}
#endif

#endif