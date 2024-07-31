#include <iostream>
#include <WS2tcpip.h>
#include <vector>
#include "ServerClass.hpp"
#define _WINSOCK_DEPRECATED_NO_WARNINGS
constexpr int PORT = 12345;
constexpr int BufferSize = 4096;

// Adding ws2_32.lib library to the linker
#pragma comment (lib, "ws2_32.lib")

// Using standard namespace
void Server()
{
    ServerClass serverClass;
    serverClass.InitializeWinsock();
    SOCKET listeningSocket = serverClass.InitializeSocket();

    // Bind the socket to an IP address and port
    sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(PORT);
    serverAddress.sin_addr.S_un.S_addr = INADDR_ANY;
    bind(listeningSocket, (sockaddr*)&serverAddress, sizeof(serverAddress));

    // Tell Winsock the socket is for listening
    listen(listeningSocket, SOMAXCONN);

    //create a Set of Sockets.
    fd_set masterSet = serverClass.CreateSocketSet();

    //add to the server the fd
    FD_SET(listeningSocket, &masterSet);

    serverClass.HandleServer(masterSet, listeningSocket);

    // Close Winsock
    WSACleanup();
}
void main()
{
    Server();
}