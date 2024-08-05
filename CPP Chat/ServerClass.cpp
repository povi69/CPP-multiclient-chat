#include "ServerClass.hpp"
#include <iostream>
#include <vector>
#define _WINSOCK_DEPRECATED_NO_WARNINGS

SOCKET clients[ServerClass::MAX_CLIENTS];
int clientCount = 0;


void ServerClass::DisplayConnectedClients(const SOCKET& currentSocket)
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
        throw std::runtime_error("Can't Initialize Winsock! Quitting");
        return;
    }
    std::cout << "Winsock initialized!\n";
}

SOCKET ServerClass::InitializeSocket()
{
    SOCKET listeningSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (listeningSocket == INVALID_SOCKET)
    {
        throw std::runtime_error("Can't Initialize Socket Quitting");

        return INVALID_SOCKET;
    }
    std::cout << "Socket Initialized!\n";
    return listeningSocket;
}

fd_set ServerClass::CreateSocketSet()
{
    fd_set masterSet;
    FD_ZERO(&masterSet);
    return masterSet;
}

void ServerClass::SendMessages(const SOCKET& currentSocket, const std::string& receiveBuffer)
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

void ServerClass::HandleServer(fd_set& masterSet, const SOCKET& listeningSocket)
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
                            SendMessages(currentSocket, std::string(receiveBuffer.data()));
                        }
                    }
                }
            }
        }
    }
}

