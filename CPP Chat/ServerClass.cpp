#include "ServerClass.hpp"
#include <iostream>
#include <vector>
#define _WINSOCK_DEPRECATED_NO_WARNINGS

SOCKET clients[ServerClass::MAX_CLIENTS];
int clientCount = 0;

void ServerClass::run()
{
    ServerClass serverClass;
    serverClass.initializeWinsock();
    SOCKET listeningSocket = serverClass.initializeSocket();

    // Bind the socket to an IP address and port
    sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(ServerClass::PORT);
    serverAddress.sin_addr.S_un.S_addr = INADDR_ANY;
    bind(listeningSocket, (sockaddr*)&serverAddress, sizeof(serverAddress));

    // Tell Winsock the socket is for listening
    listen(listeningSocket, SOMAXCONN);

    // Create a set of sockets
    fd_set masterSet = serverClass.createSocketSet();

    // Add the server's listening socket to the set
    FD_SET(listeningSocket, &masterSet);

    serverClass.handleServer(masterSet, listeningSocket);

    // Clean up Winsock
    WSACleanup();
}
void ServerClass::displayConnectedClients(const SOCKET& currentSocket)
{
    std::cout << currentSocket << " Connected!\n";
}

void ServerClass::initializeWinsock()
{
    // Initialize Winsock
    WSADATA winsockData;
    WORD winsockVersion = WINSOCK_VERSION;
    int winsockInitResult = WSAStartup(winsockVersion, &winsockData);
    if (winsockInitResult != 0)
    {
        throw std::runtime_error("Can't Initialize Winsock! Quitting");
    }
    std::cout << "Winsock initialized!\n";
}

SOCKET ServerClass::initializeSocket()
{
    SOCKET listeningSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (listeningSocket == INVALID_SOCKET)
    {
        throw std::runtime_error("Can't Initialize Socket Quitting");

    }
    std::cout << "Socket Initialized!\n";
    return listeningSocket;
}

fd_set ServerClass::createSocketSet()
{
    fd_set masterSet;
    FD_ZERO(&masterSet);
    return masterSet;
}

void ServerClass::sendMessages(const SOCKET& currentSocket, const std::string& receiveBuffer)
{
    size_t messageLength = receiveBuffer.length(); // Get the exact size of the message received

    for (int i = 0; i < clientCount; i++)
    {
        if (clients[i] != currentSocket)
        {
            int sendResult = send(clients[i], receiveBuffer.c_str(), messageLength, 0);
            if (sendResult == SOCKET_ERROR)
            {
                std::cout << "Error sending message to client socket " << clients[i] << ".\n";
            }
        }
    }
}


void ServerClass::disconnectClients(const SOCKET& currentSocket, fd_set& masterSet)
{
    closesocket(currentSocket);
    FD_CLR(currentSocket, &masterSet);
    std::cout << currentSocket << " Disconnected\n";

    for (int j = 0; j < clientCount; j++)
    {
        if (clients[j] == currentSocket)
        {
            clients[j] = clients[--clientCount]; // Replace with last client and decrease count
            break;
        }
    }
}

void ServerClass::handleServer(fd_set& masterSet, const SOCKET& listeningSocket)
{
    while (true)
    {
        fd_set copySet = masterSet;
        int readySocketCount = select(FD_SETSIZE, &copySet, nullptr, nullptr, nullptr);
        for (int i = 0; i < readySocketCount; i++)
        {
            SOCKET currentSocket = copySet.fd_array[i];

            if (currentSocket == listeningSocket)
            {
                SOCKET clientSocket = accept(listeningSocket, nullptr, nullptr);
                FD_SET(clientSocket, &masterSet);

                if (clientCount < MAX_CLIENTS)
                {
                    clients[clientCount++] = clientSocket; // Add client socket
                    displayConnectedClients(clientSocket);
                }
                else
                {
                    std::cout << "Max clients reached. Cannot accept more connections.\n";
                    closesocket(clientSocket);
                }
            }
            else
            {
                // First, receive the length of the message (4 bytes for an int)
                int messageLength = 0;
                int bytesReceived = recv(currentSocket, reinterpret_cast<char*>(&messageLength), sizeof(messageLength), 0);
                if (bytesReceived <= 0)
                {
                    disconnectClients(currentSocket, masterSet);
                }
                else
                {
                    // Now receive the actual message based on the received length
                    if (messageLength > 0)
                    {
                        std::vector<char> receiveBuffer(messageLength + 1); // +1 for \0 
                        ZeroMemory(receiveBuffer.data(), messageLength + 1);
                        bytesReceived = recv(currentSocket, receiveBuffer.data(), messageLength, 0);
                        if (bytesReceived <= 0)
                        {
                            disconnectClients(currentSocket, masterSet);
                        }
                        else
                        {
                            receiveBuffer[messageLength] = '\0';
                            std::cout << receiveBuffer.data() << std::endl;
                            sendMessages(currentSocket, std::string(receiveBuffer.data()));
                        }
                    }
                }
            }
        }
    }
}

