#include "NetHandler.h"

#include <sstream>

NetHandler::NetHandler(const NetSettings& InNetSettings) : netSettings_(InNetSettings)
{
    Initialize();
}

NetHandler::~NetHandler()
{
    // Cleanup Winsock on Windows
    #ifdef _WIN32
    WSACleanup();
    #endif
}

void NetHandler::Initialize()
{
    static bool bHasBeenSetup = false;
    if (bHasBeenSetup)
    {
        throw std::runtime_error("Winsock has already been initialized");
    }
    bHasBeenSetup = true;
    
    // Initialize Winsock on Windows
    std::cout << "Starting Winsock...";
    #ifdef _WIN32
    if (WSAStartup(MAKEWORD(2, 2), &wsaData_) != 0) {
        throw std::runtime_error("Failed to initialize Winsock");
    }
    #endif
    std::cout << "OK!" << std::endl;

    udpSocket_ = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (udpSocket_ == INVALID_SOCKET)
    {
        throw std::runtime_error("UDPSocket was invalid");
    }

    // Add server connection if valid
    if (netSettings_.serverConnectionPort != 0)
    {
        connectedParentServerAddress_.sin_family = AF_INET;
        if (InetPton(AF_INET, netSettings_.serverAddress, &connectedParentServerAddress_.sin_addr.s_addr) != 1)
        {
            throw std::runtime_error("Error converting server address");
        }

        connectedParentServerAddress_.sin_port = htons(netSettings_.serverConnectionPort);
    }
}
