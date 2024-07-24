#pragma once
#include <WS2tcpip.h>
#include <iostream>
constexpr int bufferSize = 4096;
class ServerClass
{
public:
    void DisplayConnectedClients(SOCKET currentSocket);
    void InitializeWinsock();
    SOCKET InitializeSocket();
    fd_set CreateSocketSet();
    void HandleServer(fd_set masterSet, SOCKET listeningSocket);
        std::string GetName(SOCKET currentSocket);
};
