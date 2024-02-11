#include "NetHandler.h"

#include <sstream>
#include <thread>

#include "../Packet/PacketManager.h"
#include "../Packet/CorePacketComponents/ServerConnect.hpp"
#include "../Packet/CorePacketComponents/ReturnAckComponent.hpp"

namespace Net
{
NetHandler::NetHandler(const NetSettings& InNetSettings) : netSettings_(InNetSettings),
    bIsServer_(PacketManager::Get()->GetManagerType() == ENetworkHandleType::Server)
{

}

NetHandler::~NetHandler()
{
    bIsRunning_ = false;
    if (packetListenerThread_ && packetListenerThread_->joinable())
    {
        packetListenerThread_->join();
    }
    delete packetListenerThread_;
    
    // Cleanup Winsock on Windows
    #ifdef _WIN32
    closesocket(udpSocket_);
    WSACleanup();
    #endif
}

void NetHandler::Update()
{
    ProcessPackets();
}

void NetHandler::SendPacketToTargetAndResetPacket(const NetTarget& InTarget, Packet& InPacket) const
{
    SendPacketToTarget(InTarget, InPacket);
    InPacket.Reset();
}

void NetHandler::SendPacketToTarget(const NetTarget& InTarget, const Packet& InPacket) const
{
    const sockaddr_in targetAddress = NetUtility::RetrieveIPv4AddressFromStorage(InTarget.address);
    std::cout << InPacket.GetIdentifier() << " : " << (InPacket.GetPacketType() == EPacketHandlingType::Ack ? "Ack" : "Not Ack") << " : " << "Sent Packet!\n";
    if (sendto(udpSocket_, reinterpret_cast<const char*>(&InPacket), NET_BUFFER_SIZE_TOTAL, 0, reinterpret_cast<const sockaddr*>(&targetAddress), sizeof(targetAddress)) == SOCKET_ERROR)
    {
        std::cout << "Error: " << WSAGetLastError() << '\n';
    }
}

bool NetHandler::RetrieveChildConnectionNetTargetInstance(const sockaddr_storage& InAddress, NetTarget& OutNetTarget)
{
    return connectionHandler_.GetNetTargetCopy(InAddress, OutNetTarget);
}

bool NetHandler::IsConnected(const sockaddr_storage& InAddress)
{
    return connectionHandler_.ContainsConnection(InAddress);
}

void NetHandler::Initialize()
{
    #ifdef _WIN32
    if (InitializeWin32())
    {
        packetListenerThread_ = new std::thread(&NetHandler::PacketListener, this);
    }
    #endif
}

bool NetHandler::InitializeWin32()
{
    // Initialize Winsock on Windows
    std::cout << "\nStarting Winsock...\n";
    #ifdef _WIN32
    if (WSAStartup(MAKEWORD(2, 2), &wsaData_) != 0)
    {
        std::cerr << " FAIL!" << '\n';
        std::cerr << "Error: " << WSAGetLastError() << '\n';
        return false;
    }
    #endif
    std::cout << "OK!" << '\n';

    udpSocket_ = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (udpSocket_ == INVALID_SOCKET)
    {
        std::cerr << "Failed to create udpSocket." << '\n';
        std::cerr << "Error: " << WSAGetLastError() << '\n';
        closesocket(udpSocket_);
        WSACleanup();
        return false;
    }

    // Setup parent server address
    bHasParentServer_ = netSettings_.parentServerPort != 0;
    if (bHasParentServer_)
    {
        connectedParentServerAddress_.sin_port = htons(netSettings_.parentServerPort);
        connectedParentServerAddress_.sin_family = AF_INET;
        
        if (InetPton(AF_INET, netSettings_.parentServerAddress, &connectedParentServerAddress_.sin_addr.s_addr) != 1)
        {
            std::cerr << "Parent server address and port was invalid!" << '\n';
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
            std::cerr << "Address and port was invalid!" << '\n';
            std::cerr << "Error: " << WSAGetLastError() << '\n';
            closesocket(udpSocket_);
            WSACleanup();
            return false;
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
    if (bind(udpSocket_, reinterpret_cast<sockaddr*>(&address_), sizeof(address_)) == SOCKET_ERROR)
    {
        std::cerr << "Failed to bind socket." << '\n';
        std::cerr << "Error: " << WSAGetLastError() << '\n';
        closesocket(udpSocket_);
        WSACleanup();
        return false;
    }

    // Fetch correctly assigned port by OS if client
    if (!bIsServer_)
    {
        // Retrieve the assigned port
        socklen_t len = sizeof(address_);
        if (getsockname(udpSocket_, reinterpret_cast<sockaddr*>(&address_), &len) == SOCKET_ERROR)
        {
            std::cerr << "Failed to get socket name." << '\n';
            std::cerr << "Error: " << WSAGetLastError() << '\n';
            closesocket(udpSocket_);
            WSACleanup();
            return false;
        }
    }
    
    std::cout << "Bound to port: " << ntohs(address_.sin_port) << "\n";
    
    if (!bHasParentServer_ && !bIsServer_)
    {
        std::cerr << "Needs parent server connection if not setup as server!" << '\n';
        closesocket(udpSocket_);
        WSACleanup();
        return false;
    }
    
    // Send Join Packet
    if (!bIsServer_)
    {
        parentConnection_ = NetTarget(NetUtility::RetrieveStorageFromIPv4Address(connectedParentServerAddress_));
        connectionHandler_.AddConnection(parentConnection_.address);
        
        const ServerConnectPacketComponent connectComponent;
        PacketManager::Get()->SendPacketComponent<ServerConnectPacketComponent>(connectComponent, parentConnection_);

        std::cout << "Sending join packet!\n";
    }
    
    return true;
}

void NetHandler::PacketListener(NetHandler* InNetHandler)
{
    while(InNetHandler->bIsRunning_)
    {
        sockaddr_storage senderAddress;
        int senderAddressSize = sizeof(senderAddress);

        char buffer[NET_BUFFER_SIZE_TOTAL];
        const int bytesReceived = recvfrom(InNetHandler->udpSocket_, buffer, sizeof(buffer), 0,
        reinterpret_cast<sockaddr*>(&senderAddress), &senderAddressSize);

        if (bytesReceived > 0)
        {
            InNetHandler->PreProcessPackets(buffer, bytesReceived, senderAddress); // Important: This needs update to be secure against threaded processing
            InNetHandler->UpdateNetTarget(senderAddress);
        }
        else if (bytesReceived == 0)
        {
            InNetHandler->KickNetTarget(senderAddress, ENetDisconnectType::ConnectionClosed);
            std::cerr << "Connection closed by peer.\n";
        }
        else if (bytesReceived == SOCKET_ERROR)
        {
            if (const int errorCode = WSAGetLastError(); errorCode != WSAEWOULDBLOCK) // Ignore block error TODO: Might reconsider using blocking type or not
            {
                std::cerr << "Receive failed with error: " << WSAGetLastError() << "\n";
                return;
            }
        }

        // Do other logic...
        InNetHandler->KickInactiveNetTargets();
    }
}

void NetHandler::SendReturnAckBackToNetTarget(const NetTarget& Target, const int32_t Identifier)
{
    ReturnAckComponent component;
    component.ackIdentifier = Identifier;
    PacketManager::Get()->SendPacketComponent(component, Target);
}

void NetHandler::ProcessPackets()
{
    // Retrieve and clear packets waiting for processing
    packetProcessingMutexLock_.lock();
    const std::vector<PacketProcessData> packetProcessDataCopy = packetDataToProcess_;
    packetDataToProcess_.clear();
    packetProcessingMutexLock_.unlock();

    // Process packets
    for (const PacketProcessData& data : packetProcessDataCopy)
    {
        const Packet& packet = data.packet;
        const sockaddr_storage& senderAddress = data.address;
        const NetTarget netTarget = NetTarget(senderAddress);
        
        // Handle Components
        std::vector<const PacketComponent*> outComponents;
        packet.GetComponents(outComponents);
        for (const PacketComponent* component : outComponents)
        {
            PacketManager::Get()->HandleComponent(netTarget, *component);
        }

        // Update packet tracker
        UpdatePacketTracker(senderAddress, packet.GetIdentifier());
    }
}

bool NetHandler::HandleReturnAck(const sockaddr_storage& SenderAddress, const int32_t Identifier)
{
    bool bHasAlreadyBeenReceived = false;
    if (NetTarget outNetTarget; RetrieveChildConnectionNetTargetInstance(SenderAddress, outNetTarget))
    {
        SendReturnAckBackToNetTarget(outNetTarget, Identifier);
        bHasAlreadyBeenReceived = connectionHandler_.HasPacketBeenSent(SenderAddress, Identifier);
    }
    return bHasAlreadyBeenReceived;
}

void NetHandler::UpdatePacketTracker(const sockaddr_storage& SenderAddress, const int32_t Identifier)
{
    NetTarget outNetTarget;
    if (RetrieveChildConnectionNetTargetInstance(SenderAddress, outNetTarget))
    {
        connectionHandler_.UpdatePacketTracker(SenderAddress, Identifier);
    }
}

void NetHandler::PreProcessPackets(const char* Buffer, const int BytesReceived, const sockaddr_storage& SenderAddress)
{
    const Packet packet = { Buffer, BytesReceived };

    std::cout << packet.GetIdentifier() << " : " << (packet.GetPacketType() == EPacketHandlingType::Ack ? "Ack" : "Not Ack") << " : " << "Got Packet!\n";

    // Send back response if of Ack type
    if (packet.GetPacketType() == EPacketHandlingType::Ack)
    {
        if (HandleReturnAck(SenderAddress, packet.GetIdentifier()))
        {
            return; // Packet has already been received
        }
    }

    packetProcessingMutexLock_.lock();
    packetDataToProcess_.push_back({ SenderAddress, packet });
    packetProcessingMutexLock_.unlock();
}

void NetHandler::UpdateNetTarget(const sockaddr_storage& InAddress)
{
    connectionHandler_.UpdateNetTargetClock(InAddress);
}

void NetHandler::OnChildDisconnectReceived(const NetTarget& InNetTarget, const PacketComponent& InComponent)
{
    if (connectionHandler_.ContainsConnection(InNetTarget.address))
    {
        KickNetTarget(InNetTarget.address, ENetDisconnectType::Disconnected);
    }
}

void NetHandler::OnChildConnectionReceived(const NetTarget& InNetTarget, const PacketComponent& InComponent)
{
    std::cout << "Join retrieved!\n";
    if (!IsConnected(InNetTarget.address))
    {
        connectionHandler_.AddConnection(InNetTarget.address);
        PacketManager::Get()->OnNetTargetConnected(InNetTarget);
    }
}

void NetHandler::KickInactiveNetTargets()
{
    using namespace std::chrono;
    
    constexpr int iterationAddition = static_cast<int>(1.f / NET_INACTIVE_CHECKS_PER_UPDATE_PERCENT);
    
    static int iterationOffset = 0;

    const std::vector<NetTarget> childConnections = connectionHandler_.GetCopy();
    for (size_t iter = iterationOffset; iter < childConnections.size(); iter += iterationAddition)
    {
        const NetTarget& netTarget = childConnections[iter];

        const steady_clock::time_point currentTime = steady_clock::now();
        const duration<float> timeDifference = duration_cast<duration<float>>(currentTime - netTarget.lastTimeReceivedNetEvent);
        if (timeDifference.count() > NET_TIME_UNTIL_TIME_OUT_SEC)
        {
            KickNetTarget(netTarget.address, ENetDisconnectType::TimeOut);
        }
    }

    ++iterationOffset;
    if (iterationOffset == iterationAddition)
    {
        iterationOffset = 0;
    }
}

void NetHandler::KickNetTarget(const sockaddr_storage& InAddress, const ENetDisconnectType InKickReason)
{
    NetTarget outNetTarget;
    if (connectionHandler_.GetNetTargetCopy(InAddress, outNetTarget))
    {
        PacketManager::Get()->OnNetTargetDisconnection(outNetTarget, InKickReason);
        connectionHandler_.RemoveConnection(InAddress);
    }
}
}