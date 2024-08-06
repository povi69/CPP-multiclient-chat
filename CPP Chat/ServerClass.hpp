#pragma once
#include <WS2tcpip.h>
#include <iostream>


class ServerClass
{
private: 
public:
    static constexpr auto MAX_CLIENTS = 64;
    static constexpr auto PORT = 12345;

    void run();
    /**
     * @brief Displays the information of the connected client.
     * @param currentSocket The socket of the currently connected client.
     */
    void displayConnectedClients(const SOCKET& currentSocket);

    void initializeWinsock();

    SOCKET initializeSocket();

    /**
     * @brief Creates an empty set of sockets.
     * @return An empty fd_set structure.
     */
    fd_set createSocketSet();

    /**
     * @brief Handles incoming client connections and messages.
     * @param masterSet The set of sockets to monitor for incoming connections and messages.
     * @param listeningSocket The socket that listens for new client connections.
     */
    void handleServer(fd_set& masterSet, const SOCKET& listeningSocket);

    /**
     * @brief Sends messages to all connected clients except the sender.
     * @param currentSocket The socket of the client sending the message.
     * @param receiveBuffer The message to be sent.
     */
    void sendMessages(const SOCKET& currentSocket, const std::string& receiveBuffer);

    /**
     * @brief Disconnects a client and removes it from the set of monitored sockets.
     * @param currentSocket The socket of the client to be disconnected.
     * @param masterSet The set of sockets being monitored.
     */
    void disconnectClients(const SOCKET& currentSocket, fd_set& masterSet);
};
