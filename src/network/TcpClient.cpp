#include "network/TcpClient.h"
#include <iostream>
#ifdef _WIN32
#include <ws2tcpip.h>
#else
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#endif
#include <cstring>

namespace
{
void closeSocket(SocketHandle socket)
{
#ifdef _WIN32
    closesocket(socket);
#else
    close(socket);
#endif
}
}

TcpClient::TcpClient(const std::string& serverIP, int port)
    : clientSocket(InvalidSocket),
      serverIP(serverIP),
      port(port)
{
    initializeSockets();
}

bool TcpClient::connectToServer() {
    clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if(clientSocket == InvalidSocket) {
        std::cerr << "Failed to create socket. \n";
        return false;
    }
    sockaddr_in serverAddress{};
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(port);

    if(inet_pton(AF_INET, serverIP.c_str(), &serverAddress.sin_addr) <= 0) {
        std::cerr << "Invalid server address.\n";
        closeSocket(clientSocket);
        return false;
    }

    if(connect(clientSocket, reinterpret_cast<sockaddr*>(&serverAddress), sizeof(serverAddress)) == SOCKET_ERROR) {
        std::cerr << "Connection failed. \n";
        closeSocket(clientSocket);
        return false;
    }
    std::cout << "Connected to server." << std::endl;
    return true;
}

void TcpClient::disconnect() {
    if(clientSocket != InvalidSocket) {
        closeSocket(clientSocket);
    }
}

bool TcpClient::sendMessage(const std::string& message) {
    return send(clientSocket, message.c_str(), static_cast<int>(message.size()), 0) != SOCKET_ERROR;
}

bool TcpClient::receiveMessage(std::string& message) {
    char buffer[1024] = {};
    int bytesReceived = recv(clientSocket, buffer, sizeof(buffer)-1, 0);
    
    if(bytesReceived <= 0) return false;
    buffer[bytesReceived] = '\0';
    message = buffer;
    
    return true;
}

