#ifndef TCPSERVER_H
#define TCPSERVER_H

#include <string>
#include "network/SocketTypes.h"

class TcpServer
{
private:
    SocketHandle serverSocket;
    int port;

public:
    explicit TcpServer(int port);

    bool start();

    SocketHandle acceptClient();

    bool receiveMessage(
        SocketHandle clientSocket,
        std::string& message,
        std::string& receiveBuffer
    );

    bool sendMessage(
        SocketHandle clientSocket,
        const std::string& message
    );

    void closeClient(SocketHandle clientSocket);

    void stop();
};

#endif