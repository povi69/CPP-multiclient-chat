#include <iostream>
#include <WS2tcpip.h>
#include <vector>
#include "ServerClass.hpp"
#define _WINSOCK_DEPRECATED_NO_WARNINGS
// Adding ws2_32.lib library to the linker
#pragma comment (lib, "ws2_32.lib")




int main()
{
    try {
        ServerClass serverClass;
        serverClass.run();
    }
    catch(const std::exception& e)
    {
        std::cout << "Exception catch: " << e.what();
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
