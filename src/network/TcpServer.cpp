#include "network/TcpServer.h"

#include <iostream>
#include <cstring>

#ifdef _WIN32
#include <ws2tcpip.h>
#else
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#endif

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

TcpServer::TcpServer(int port)
    : serverSocket(InvalidSocket),
      port(port)
{
    initializeSockets();
}

bool TcpServer::start()
{
    serverSocket = socket(AF_INET, SOCK_STREAM, 0);

    if (serverSocket == InvalidSocket)
    {
        std::cerr << "Failed to create socket.\n";
        return false;
    }

    int reuse = 1;

#ifdef _WIN32
    setsockopt(
        serverSocket,
        SOL_SOCKET,
        SO_REUSEADDR,
        reinterpret_cast<const char*>(&reuse),
        sizeof(reuse)
    );
#else
    setsockopt(
        serverSocket,
        SOL_SOCKET,
        SO_REUSEADDR,
        &reuse,
        sizeof(reuse)
    );
#endif

    sockaddr_in serverAddress{};

    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = INADDR_ANY;
    serverAddress.sin_port = htons(port);

    if (bind(
            serverSocket,
            reinterpret_cast<sockaddr*>(&serverAddress),
            sizeof(serverAddress)) == SOCKET_ERROR)
    {
        std::cerr << "Bind failed.\n";
        closeSocket(serverSocket);
        serverSocket = InvalidSocket;
        return false;
    }

    if (listen(serverSocket, 64) == SOCKET_ERROR)
    {
        std::cerr << "Listen failed.\n";
        closeSocket(serverSocket);
        serverSocket = InvalidSocket;
        return false;
    }

    return true;
}

SocketHandle TcpServer::acceptClient()
{
    sockaddr_in clientAddress{};

#ifdef _WIN32
    int clientLength = sizeof(clientAddress);
#else
    socklen_t clientLength = sizeof(clientAddress);
#endif

    SocketHandle clientSocket = accept(
        serverSocket,
        reinterpret_cast<sockaddr*>(&clientAddress),
        &clientLength
    );

    if (clientSocket == InvalidSocket)
    {
        std::cerr << "Failed to accept client.\n";
        return InvalidSocket;
    }

    return clientSocket;
}

bool TcpServer::receiveMessage(
    SocketHandle clientSocket,
    std::string& message,
    std::string& receiveBuffer)
{
    char buffer[4096];

    while (true)
    {
        const std::size_t newlinePosition =
            receiveBuffer.find('\n');

        if (newlinePosition != std::string::npos)
        {
            message = receiveBuffer.substr(0, newlinePosition);

            receiveBuffer.erase(
                0,
                newlinePosition + 1
            );

            if (!message.empty() && message.back() == '\r')
            {
                message.pop_back();
            }

            return true;
        }

        const int bytesReceived = recv(
            clientSocket,
            buffer,
            sizeof(buffer),
            0
        );

        if (bytesReceived <= 0)
        {
            return false;
        }

        receiveBuffer.append(
            buffer,
            static_cast<std::size_t>(bytesReceived)
        );
    }
}

bool TcpServer::sendMessage(
    SocketHandle clientSocket,
    const std::string& message)
{
    std::string framedMessage = message;

    if (framedMessage.empty() || framedMessage.back() != '\n')
    {
        framedMessage += '\n';
    }

    const char* data = framedMessage.c_str();
    std::size_t totalSent = 0;

    while (totalSent < framedMessage.size())
    {
        const int bytesSent = send(
            clientSocket,
            data + totalSent,
            static_cast<int>(framedMessage.size() - totalSent),
            0
        );

        if (bytesSent <= 0)
        {
            return false;
        }

        totalSent += static_cast<std::size_t>(bytesSent);
    }

    return true;
}

void TcpServer::closeClient(SocketHandle clientSocket)
{
    if (clientSocket == InvalidSocket)
    {
        return;
    }

    closeSocket(clientSocket);
}

void TcpServer::stop()
{
    if (serverSocket != InvalidSocket)
    {
        closeSocket(serverSocket);
        serverSocket = InvalidSocket;
    }

#ifdef _WIN32
    WSACleanup();
#endif
}