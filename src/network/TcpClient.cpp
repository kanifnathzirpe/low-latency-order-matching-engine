#include "network/TcpClient.h"
#include <iostream>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <cstring>

TcpClient::TcpClient(const std::string& serverIP, int port)
    : clientSocket(-1),
      serverIP(serverIP),
      port(port)
{}

bool TcpClient::connectToServer() {
    clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if(clientSocket < 0) {
        std::cerr << "Failed to create socket. \n";
        return false;
    }
    sockaddr_in serverAddress{};
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(port);

    if(inet_pton(AF_INET, serverIP.c_str(), &serverAddress.sin_addr) <= 0) {
        std::cerr << "Invalid server address.\n";
        close(clientSocket);
        return false;
    }

    if(connect(clientSocket, reinterpret_cast<sockaddr*>(&serverAddress), sizeof(serverAddress)) < 0) {
        std::cerr << "Connection failed. \n";
        close(clientSocket);
        return false;
    }
    std::cout << "Connected to server." << std::endl;
    return true;
}

void TcpClient::disconnect() {
    if(clientSocket >= 0) {
        close(clientSocket);
    }
}

bool TcpClient::sendMessage(const std::string& message) {
    return send(clientSocket, message.c_str(), message.size(), 0)>=0;
}

bool TcpClient::receiveMessage(std::string& message) {
    char buffer[1024] = {};
    ssize_t bytesReceived = recv(clientSocket, buffer, sizeof(buffer)-1, 0);
    
    if(bytesReceived <= 0) return false;
    buffer[bytesReceived] = '\0';
    message = buffer;
    
    return true;
}

