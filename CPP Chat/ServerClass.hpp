#pragma once
#include <WS2tcpip.h>
#include <iostream>


/**
 * @brief Class for managing server operations including initializing Winsock, handling client connections,sending
 * and receiving messages.
 *
 */
class ServerClass
{
public:
    /**
    * @brief The size of the buffer used for receiving messages.
    */
    static constexpr int bufferSize = 4096;

    /**
     * @brief Displays the information of the connected client.
     * @param currentSocket The socket of the currently connected client.
     */
    void DisplayConnectedClients(SOCKET currentSocket);

    /**
     * @brief Initializes the Winsock library.
     */
    void InitializeWinsock();

    /**
     * @brief Creates and initializes a listening socket for the server.
     * @return The initialized socket. Returns INVALID_SOCKET if an error occurs.
     */
    SOCKET InitializeSocket();

    /**
     * @brief Creates an empty set of sockets.
     * @return An empty fd_set structure.
     */
    fd_set CreateSocketSet();

    /**
     * @brief Handles incoming client connections and messages.
     * @param masterSet The set of sockets to monitor for incoming connections and messages.
     * @param listeningSocket The socket that listens for new client connections.
     */
    void HandleServer(fd_set masterSet, SOCKET listeningSocket);

    /**
     * @brief Sends messages to all connected clients except the sender.
     * @param currentSocket The socket of the client sending the message.
     * @param receiveBuffer The message to be sent.
     */
    void SendMessages(SOCKET currentSocket, std::string receiveBuffer);

    /**
     * @brief Disconnects a client and removes it from the set of monitored sockets.
     * @param currentSocket The socket of the client to be disconnected.
     * @param masterSet The set of sockets being monitored.
     */
    void disconnectClients(SOCKET currentSocket, fd_set masterSet);
};

