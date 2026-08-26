#include "network/TcpServer.h"
#include <iostream>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <cstring>

TcpServer::TcpServer(int port)
    : serverSocket(-1), 
      clientSocket(-1), 
      port(port) 
{}

bool TcpServer::start() {
    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket < 0) {
        std::cerr << "Failed to create socket." << std::endl;
        return false;
    }
    
    sockaddr_in serverAddress{};
    
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = INADDR_ANY;
    serverAddress.sin_port = htons(port);

    if(bind(serverSocket, reinterpret_cast<sockaddr*>(&serverAddress), sizeof(serverAddress)) < 0) {
        std::cerr << "Bind failed. \n";
        close(serverSocket);
        return false;
    }
    if(listen(serverSocket, 5) < 0) {
        std::cerr << "Listen failed. \n";
        close(serverSocket);
        return false;
    }
    return true;
}

bool TcpServer::acceptClient() {
    sockaddr_in clientAddress{};
    socklen_t clientLength = sizeof(clientAddress);

    clientSocket = accept(
        serverSocket,
        reinterpret_cast<sockaddr*>(&clientAddress),
        &clientLength
    );

    if (clientSocket < 0)
    {
        std::cerr << "Failed to accept client.\n";
        return false;
    }

    return true;
}

void TcpServer::stop() {
    if(clientSocket >= 0)
        close(clientSocket);
    
    if(serverSocket >= 0)
        close(serverSocket);
}

bool TcpServer::receiveMessage(std::string& message) {
    char buffer[1024] = {};
    ssize_t bytesReceived = recv(clientSocket, buffer, sizeof(buffer)-1, 0);
    
    if(bytesReceived <= 0) return false;
    buffer[bytesReceived] = '\0';
    message = buffer;
    
    return true;
}

bool TcpServer::sendMessage(const std::string& message) {
    return send(clientSocket, message.c_str(), message.size(), 0)>=0;
}

