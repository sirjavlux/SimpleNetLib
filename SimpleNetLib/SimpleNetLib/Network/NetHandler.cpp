#include "NetHandler.h"

#include <sstream>
#include <thread>

#include "../Packet/PacketManager.h"
#include "../Packet/CorePacketComponents/ServerConnect.hpp"
#include "../Packet/CorePacketComponents/ReturnAckComponent.hpp"

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

void NetHandler::SendPacketToTargetAndResetPacket(const NetTarget& InTarget, Packet& InPacket) const
{
    SendPacketToTarget(InTarget, InPacket);
    InPacket.Reset();
}

void NetHandler::SendPacketToTarget(const NetTarget& InTarget, const Packet& InPacket) const
{
    const sockaddr_in targetAddress = Net::RetrieveIPv4AddressFromStorage(InTarget.address);
    std::cout << InPacket.GetIdentifier() << " : " << (InPacket.GetPacketType() == EPacketHandlingType::Ack ? "Ack" : "Not Ack") << " : " << "Sent Packet!\n";
    if (sendto(udpSocket_, reinterpret_cast<const char*>(&InPacket), NET_BUFFER_SIZE_TOTAL, 0, reinterpret_cast<const sockaddr*>(&targetAddress), sizeof(targetAddress)) == SOCKET_ERROR)
    {
        std::cout << "Error: " << WSAGetLastError() << '\n';
    }
}

bool NetHandler::RetrieveChildConnectionNetTargetInstance(const sockaddr_storage& InAddress, NetTarget*& OutNetTarget)
{
    const auto searchResult = std::find(childConnections_.begin(), childConnections_.end(), NetTarget(InAddress));
    if (searchResult != childConnections_.end())
    {
        OutNetTarget = searchResult._Ptr;
        return true;
    }
    return false;
}

bool NetHandler::IsConnected(const sockaddr_storage& InAddress)
{
    const auto searchResult = std::find(childConnections_.begin(), childConnections_.end(), NetTarget(InAddress));
    return searchResult != childConnections_.end();
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
    if (bind(udpSocket_, reinterpret_cast<sockaddr*>(&address_), sizeof address_) == SOCKET_ERROR)
    {
        std::cerr << "Failed to bind socket." << '\n';
        std::cerr << "Error: " << WSAGetLastError() << '\n';
        closesocket(udpSocket_);
        WSACleanup();
        return false;
    }
    
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
        parentConnection_ = NetTarget(Net::RetrieveStorageFromIPv4Address(connectedParentServerAddress_));

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
            InNetHandler->ProcessPackets(buffer, bytesReceived, senderAddress);
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

bool NetHandler::HandleReturnAck(const sockaddr_storage& SenderAddress, const int32_t Identifier)
{
    std::cout << "Handle Ack Return\n";
    
    bool bHasAlreadyBeenReceived = false;
    // Client receiving packet
    if (parentConnection_ == SenderAddress)
    {
        std::cout << "Found Parent Connection\n";
        SendReturnAckBackToNetTarget(parentConnection_, Identifier);
        bHasAlreadyBeenReceived = parentConnection_.HasPacketBeenSent(Identifier);
    }
    // Server receiving packet
    else  if (NetTarget* outNetTarget; RetrieveChildConnectionNetTargetInstance(SenderAddress, outNetTarget))
    {
        std::cout << "Found Child Connection\n";
        SendReturnAckBackToNetTarget(*outNetTarget, Identifier);
        bHasAlreadyBeenReceived = outNetTarget->HasPacketBeenSent(Identifier);
    }
    return bHasAlreadyBeenReceived;
}
void NetHandler::UpdatePacketTracker(const sockaddr_storage& SenderAddress, const int32_t Identifier)
{
    // Client receiving packet
    if (parentConnection_ == SenderAddress)
    {
        parentConnection_.UpdatePacketTracker(Identifier);
    }
    // Server receiving packet
    else  if (NetTarget* outNetTarget; RetrieveChildConnectionNetTargetInstance(SenderAddress, outNetTarget))
    {
        outNetTarget->UpdatePacketTracker(Identifier);
    }
}

void NetHandler::ProcessPackets(const char* Buffer, const int BytesReceived, const sockaddr_storage& SenderAddress)
{
    // TODO: Packet processing, acks, ack returns, statistics, etc...
    Packet packet = { Buffer, BytesReceived };

    std::cout << packet.GetIdentifier() << " : " << (packet.GetPacketType() == EPacketHandlingType::Ack ? "Ack" : "Not Ack") << " : " << "Got Packet!\n";

    // Send back response if of Ack type
    if (packet.GetPacketType() == EPacketHandlingType::Ack)
    {
        if (HandleReturnAck(SenderAddress, packet.GetIdentifier()))
        {
            std::cout << "Has already been received\n";
            return; // Packet has already been received
        }
    }

    // Handle Components
    std::vector<PacketComponent*> outComponents;
    packet.GetComponents(outComponents);
    for (const PacketComponent* component : outComponents)
    {
        PacketManager::Get()->HandleComponent(NetTarget(SenderAddress), *component);
    }

    // Update packet tracker
    UpdatePacketTracker(SenderAddress, packet.GetIdentifier());
}

void NetHandler::UpdateNetTarget(const sockaddr_storage& InAddress)
{
    using namespace std::chrono;

    if (NetTarget* outNetTarget; RetrieveChildConnectionNetTargetInstance(InAddress, outNetTarget))
    {
        outNetTarget->lastTimeReceivedNetEvent = steady_clock::now();
    }
}

void NetHandler::OnChildDisconnectReceived(const NetTarget& InNetTarget, const ServerDisconnectPacketComponent& InComponent)
{
    if (IsConnected(InNetTarget.address))
    {
        KickNetTarget(InNetTarget.address, ENetDisconnectType::Disconnected);
    }
}

void NetHandler::OnChildConnectionReceived(const NetTarget& InNetTarget, const ServerConnectPacketComponent& InComponent)
{
    if (!IsConnected(InNetTarget.address))
    {
        childConnections_.push_back(InNetTarget);

        PacketManager::Get()->OnNetTargetConnected(InNetTarget);
    }
}

void NetHandler::KickInactiveNetTargets()
{
    using namespace std::chrono;
    
    constexpr int iterationAddition = static_cast<int>(1.f / NET_INACTIVE_CHECKS_PER_UPDATE_PERCENT);
    
    static int iterationOffset = 0;
    
    for (size_t iter = iterationOffset; iter < childConnections_.size(); iter += iterationAddition)
    {
        const NetTarget& netTarget = childConnections_[iter];

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
    NetTarget* outNetTarget;
    if (RetrieveChildConnectionNetTargetInstance(InAddress, outNetTarget))
    {
        PacketManager::Get()->OnNetTargetDisconnection(*outNetTarget, InKickReason);
        childConnections_.erase(std::find(childConnections_.begin(), childConnections_.end(), *outNetTarget));
    }
}
