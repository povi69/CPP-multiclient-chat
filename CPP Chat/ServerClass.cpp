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
}

SOCKET ServerClass::InitializeSocket()
{
    SOCKET listeningSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (listeningSocket == INVALID_SOCKET)
    {
        std::cout << "Can't create a socket! Quitting\n";
        return INVALID_SOCKET;
    }
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
    char receivedName[bufferSize];
    const char* enterNameMessage = "Enter your name: ";
    send(currentSocket, enterNameMessage, strlen(enterNameMessage), 0);
    int bytesReceived = recv(currentSocket, receivedName, bufferSize - 1, 0);
    //put null terminator at the end of the message received
    if (bytesReceived > 0)
    {
        receivedName[bytesReceived] = '\0';
    }
    return std::string(receivedName);
}

void ServerClass::SendMessage(SOCKET currentSocket,SOCKET listeningSocket,char* receiveBuffer)
{

    for (int j = 0; j < clientCount; j++)
    {
        if (clients[j].socket == currentSocket)
        {
            std::string nameAndMessageCombined = clients[j].name + " : " + std::string(receiveBuffer);
            std::cout << "Message from " << nameAndMessageCombined << "\n";
            const char* confirmationMessage = "Message sent!\r\n";
            send(currentSocket, confirmationMessage, strlen(confirmationMessage), 0);

            for (int k = 0; k < clientCount; k++)
            {
                if (clients[k].socket != listeningSocket && clients[k].socket != currentSocket)
                {
                    send(clients[k].socket, nameAndMessageCombined.c_str(), nameAndMessageCombined.length(), 0);
                }
            }
            break;
        }
    }
}
void ServerClass::DisconnectSocket(SOCKET currentSocket,fd_set masterSet)
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
                    std::string clientName = GetName(clientSocket);
                    clients[clientCount++] = { clientSocket, clientName };
                    DisplayConnectedClients(clientSocket);
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
                    DisconnectSocket(currentSocket, masterSet);
                }
                else
                {
                    SendMessage(currentSocket, listeningSocket, receiveBuffer);
                }
            }
        }
    }
}
