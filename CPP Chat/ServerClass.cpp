#include "ServerClass.hpp"
#include <iostream>

#define _WINSOCK_DEPRECATED_NO_WARNINGS

#define MAX_CLIENTS 64

struct SocketStruct
{
    SOCKET socket;  // The user's connected socket
    std::string name;    // The name of the connected user
};

SocketStruct clients[MAX_CLIENTS];
int clientCount = 0;

void ServerClass::DisplayConnectedClients(SOCKET currentSocket)
{
    std::cout << currentSocket << " Connected!\n";
}

void ServerClass::InitializeWinsock()
{
    // Initialize Winsock
    WSADATA winsockData;
    WORD winsockVersion = MAKEWORD(2, 2);
    int winsockInitResult = WSAStartup(winsockVersion, &winsockData);
    if (winsockInitResult != 0)
    {
        std::cout << "Can't Initialize Winsock! Quitting\n";
        return;
    }
    std::cout << "Winsock initialized!\n";
}

SOCKET ServerClass::InitializeSocket()
{
    SOCKET listeningSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (listeningSocket == INVALID_SOCKET)
    {
        std::cout << "Can't create a socket! Quitting\n";
        return INVALID_SOCKET;
    }
    std::cout << "Socket Initialized!\n";
    return listeningSocket;
}

fd_set ServerClass::CreateSocketSet()
{
    // fd = File Discriptor. 
    fd_set masterSet;
    FD_ZERO(&masterSet);
    return masterSet;
}

std::string ServerClass::GetName(SOCKET currentSocket)
{
    return "Ignore";
}

void ServerClass::SendMessages(SOCKET currentSocket, SOCKET listeningSocket, std::string receiveBuffer)
{
    send(currentSocket, receiveBuffer.c_str(),receiveBuffer.length(), 0);
}
void ServerClass::disconnectClients(SOCKET currentSocket,fd_set masterSet)
{
    closesocket(currentSocket);
    FD_CLR(currentSocket, &masterSet);
    std::cout << currentSocket << " Disconnected\n";

    for (int j = 0; j < clientCount; j++)
    {
        if (clients[j].socket == currentSocket)
        {
            clients[j] = clients[--clientCount]; // Replace with last client
            break;
        }
    }
}

void ServerClass::HandleServer(fd_set masterSet, SOCKET listeningSocket)
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
                // Accept a new connection
                SOCKET clientSocket = accept(listeningSocket, nullptr, nullptr);
                FD_SET(clientSocket, &masterSet);

                if (clientCount < MAX_CLIENTS)
                {
                    DisplayConnectedClients(clientSocket);
                    std::string clientName = GetName(clientSocket);
                    clients[clientCount++] = { clientSocket, clientName };
                }
                else
                {
                    std::cout << "Max clients reached. Cannot accept more connections.\n";
                    closesocket(clientSocket);
                }
            }
            else
            {
                char receiveBuffer[bufferSize];
                ZeroMemory(receiveBuffer, bufferSize);

                int bytesReceived = recv(currentSocket, receiveBuffer, bufferSize - 1, 0);
                if (bytesReceived <= 0)
                {
                    disconnectClients(currentSocket, masterSet);
                }
                else
                {
                    std::cout << receiveBuffer << std::endl;
                    SendMessages(currentSocket, listeningSocket, receiveBuffer);
                }
            }
        }
    }
}