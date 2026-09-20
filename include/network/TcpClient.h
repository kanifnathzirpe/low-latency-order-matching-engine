#ifndef TCPCLIENT_H
#define TCPCLIENT_H

#include <string>
#include "network/SocketTypes.h"

class TcpClient {
    private:
        SocketHandle clientSocket;
        std::string serverIP;
        int port;
    
    public:
        TcpClient(const std::string& serverIP, int port);
        bool connectToServer();
        void disconnect();
        bool sendMessage(const std::string& message);
        bool receiveMessage(std::string& message);
};

#endif