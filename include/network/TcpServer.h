#ifndef TCPSERVER_H
#define TCPSERVER_H

#include <string>
class TcpServer {
    private:
        int serverSocket;
        int clientSocket;
        int port;
    public:
        explicit TcpServer(int port);

        bool start();

        bool acceptClient();

        void stop();

        bool receiveMessage(std::string& message);

        bool sendMessage(const std::string& message);
};

#endif

