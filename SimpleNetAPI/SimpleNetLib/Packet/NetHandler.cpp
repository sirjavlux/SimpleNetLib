#include "NetHandler.h"

#include <sstream>

#include "PacketManager.h"

NetHandler::NetHandler(const NetSettings& InNetSettings) : netSettings_(InNetSettings),
    bIsServer_(PacketManager::Get()->GetManagerType() == ENetworkHandleType::Server)
{
    #ifdef _WIN32
    InitializeWin32();
    #endif
}

NetHandler::~NetHandler()
{
    // Cleanup Winsock on Windows
    #ifdef _WIN32
    WSACleanup();
    #endif
}

void NetHandler::InitializeWin32()
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
        std::cout << " FAIL!" << std::endl;
        std::cout << "Error: " << WSAGetLastError() << std::endl;
        return;
    }
    #endif
    std::cout << "OK!" << std::endl;

    udpSocket_ = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (udpSocket_ == INVALID_SOCKET)
    {
        std::cout << "Failed to create udpSocket." << std::endl;
        std::cout << "Error: " << WSAGetLastError() << std::endl;
        closesocket(udpSocket_);
        WSACleanup();
        return;
    }

    // Setup parent server address
    bHasParentServer_ = netSettings_.serverPort != 0;
    if (bHasParentServer_)
    {
        connectedParentServerAddress_.sin_port = htons(netSettings_.serverPort);
        connectedParentServerAddress_.sin_family = AF_INET;
        
        if (InetPton(AF_INET, netSettings_.serverAddress, &connectedParentServerAddress_.sin_addr.s_addr) != 1)
        {
            std::cout << "Parent server address and port was invalid!" << std::endl;
            bHasParentServer_ = false;
        }
    }

    // Set own address
    if (bIsServer_)
    {
        address_.sin_port = htons(netSettings_.serverPort);
        address_.sin_family = AF_INET;
        
        if (InetPton(AF_INET, netSettings_.serverAddress, &address_.sin_addr.s_addr) != 1)
        {
            std::cout << "Address and port was invalid!" << std::endl;
            std::cout << "Error: " << WSAGetLastError() << std::endl;
            closesocket(udpSocket_);
            WSACleanup();
            return;
        }
    }
    else
    {
        address_.sin_family = AF_INET;
        address_.sin_addr.s_addr = INADDR_ANY;
        address_.sin_port = htons(0); // Bind to any available port
    }
    
    // 1 for non-blocking, 0 for blocking
    u_long iMode = 1;
    ioctlsocket(udpSocket_, FIONBIO, &iMode);

    BOOL bNewBehavior = FALSE;
    DWORD dwBytesReturned = 0;
    WSAIoctl(
        udpSocket_,
        SIO_UDP_CONNRESET,
        &bNewBehavior,
        sizeof bNewBehavior,
        nullptr,
        0,
        &dwBytesReturned,
        nullptr,
        nullptr);

    // Bind socket
    if (bind(udpSocket_, reinterpret_cast<sockaddr*>(&address_), sizeof address_) == SOCKET_ERROR)
    {
        std::cout << "Failed to bind socket." << std::endl;
        std::cout << "Error: " << WSAGetLastError() << std::endl;
        closesocket(udpSocket_);
        WSACleanup();
        return;
    }
}
