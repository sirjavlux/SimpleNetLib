#include "NetHandler.h"

#include <sstream>
#include <thread>

#include "../Events/EventSystem.h"

#include "../Packet/PacketManager.h"
#include "../Packet/CorePacketComponents/ServerConnectPacketComponent.hpp"
#include "../Packet/CorePacketComponents/ReturnAckComponent.hpp"
#include "../Packet/CorePacketComponents/SuccessfullyConnectedToServer.hpp"
#include "../Packet/CorePacketComponents/KickedFromServerPacketComponent.hpp"
#include "../Packet/CorePacketComponents/ServerPingPacketComponent.hpp"
#include "../Packet/CorePacketComponents/DataReplicationPacketComponent.hpp"
#include "../Packet/CorePacketComponents/ServerDisconnectPacketComponent.hpp"
#include "../Utility/StringUtility.hpp"

namespace Net
{
NetHandler::NetHandler()
{

}

NetHandler::~NetHandler()
{
    StopAndCleanUpConnection();
}

void NetHandler::Initialize()
{
    {
        const PacketComponentAssociatedData associatedData = PacketComponentAssociatedData
        {
            false,
            18,
            EPacketHandlingType::Ack
        };
        SimpleNetLibCore::Get()->GetPacketComponentRegistry()->RegisterPacketComponent<ServerDisconnectPacketComponent, NetHandler>
            (associatedData, &NetHandler::OnChildDisconnectReceived, this);
        SimpleNetLibCore::Get()->GetPacketComponentRegistry()->RegisterPacketComponent<ServerConnectPacketComponent, NetHandler>
            (associatedData, &NetHandler::OnChildConnectionReceived, this);
        SimpleNetLibCore::Get()->GetPacketComponentRegistry()->RegisterPacketComponent<SuccessfullyConnectedToServer, NetHandler>
            (associatedData, &NetHandler::OnConnectionToServerSuccessful, this);
    }
    {
        const PacketComponentAssociatedData associatedData = PacketComponentAssociatedData
        {
            false,
            30,
            EPacketHandlingType::Ack
        };
        SimpleNetLibCore::Get()->GetPacketComponentRegistry()->RegisterPacketComponent<ServerPingPacketComponent>(associatedData);
    }
    {
        const PacketComponentAssociatedData associatedData = PacketComponentAssociatedData
        {
            false,
            1,
            EPacketHandlingType::Ack
        };
        SimpleNetLibCore::Get()->GetPacketComponentRegistry()->RegisterPacketComponent<DataReplicationPacketComponent>(associatedData);
    }
    {
        const PacketComponentAssociatedData associatedData = PacketComponentAssociatedData
        {
            false,
            6,
            EPacketHandlingType::None
        };
        SimpleNetLibCore::Get()->GetPacketComponentRegistry()->RegisterPacketComponent<ReturnAckComponent>(associatedData);
        SimpleNetLibCore::Get()->GetPacketComponentRegistry()->RegisterPacketComponent<KickedFromServerPacketComponent>(associatedData);
    }
    
    SimpleNetLibCore::Get()->GetPacketComponentDelegator()->SubscribeToPacketComponentDelegate
        <KickedFromServerPacketComponent, NetHandler>(&NetHandler::OnKickedFromServerReceived, this);
}

void NetHandler::Update()
{
    if (!bIsRunning_)
        return;

    // Check Time Out
    using namespace std::chrono;

    if (!IsServer())
    {
        const steady_clock::time_point currentTime = steady_clock::now();
        const duration<float> timeDifference = duration_cast<duration<float>>(currentTime - serverConnectionTimePoint_);
        if (timeDifference.count() > NET_TIME_UNTIL_TIME_OUT_SEC)
        {
            DisconnectFromServer();
            std::cout << "Timed out, no connection.\n";
            return;
        }

        if (bShouldDisconnect_ == true)
        {
            bShouldDisconnect_ = false;
            
            DisconnectFromServer();
            std::cout << "Disconnected from server.\n";
        }
    }
    
    ProcessPackets();

    // Can only kick connections downstream
    if (bIsServer_)
    {
        KickInactiveNetTargets();
    }
}

void NetHandler::SendPacketToTargetAndResetPacket(const sockaddr_storage& InTarget, Packet& InPacket) const
{
    SendPacketToTarget(InTarget, InPacket);
    InPacket.Reset();
}

void NetHandler::SendPacketToTarget(const sockaddr_storage& InTarget, const Packet& InPacket) const
{
    const sockaddr_in targetAddress = NetUtility::RetrieveIPv4AddressFromStorage(InTarget);
    if (sendto(udpSocket_, reinterpret_cast<const char*>(&InPacket), NET_BUFFER_SIZE_TOTAL, 0, reinterpret_cast<const sockaddr*>(&targetAddress), sizeof(targetAddress)) == SOCKET_ERROR)
    {
        std::cout << "Error: " << WSAGetLastError() << '\n';
    }

    //std::cout << InPacket.GetIdentifier() << " : " << (InPacket.GetPacketType() == EPacketHandlingType::Ack ? "Ack" : "Not Ack") << " : " << "Sent Packet!\n"; // Temporary Debug
}

const NetTarget* NetHandler::RetrieveChildConnectionNetTargetInstance(const sockaddr_storage& InAddress) const
{
    return connectionHandler_.GetNetTarget(InAddress);
}

bool NetHandler::IsConnected(const sockaddr_storage& InAddress)
{
    return connectionHandler_.ContainsConnection(InAddress);
}

void NetHandler::SetUpServer(const PCSTR InServerAddress, const u_short InServerPort)
{
    if (!bIsRunning_)
    {
        NetSettings netSettings;
        netSettings.serverAddress = InServerAddress;
        netSettings.serverPort = InServerPort;

        bIsServer_ = true;
        
        #ifdef _WIN32
        if (InitializeWin32(netSettings))
        {
            packetListenerThread_ = new std::thread(&NetHandler::PacketListener, this);
            PacketManager::Get()->managerType_ = ENetworkHandleType::Server;
            bIsRunning_ = true;
        }
        #endif
    }
}

void NetHandler::ConnectToServer(const VariableDataObject<CONNECTION_COMPONENT_DATA_SIZE>& InVariableData, const PCSTR InServerAddress, const u_short InServerPort)
{
    if (!bIsRunning_)
    {
        NetSettings netSettings;
        netSettings.parentServerAddress = InServerAddress;
        netSettings.parentServerPort = InServerPort;

        bIsServer_ = false;
        
        #ifdef _WIN32
        if (InitializeWin32(netSettings))
        {
            SendConnectionPacket(InVariableData);
            
            packetListenerThread_ = new std::thread(&NetHandler::PacketListener, this);
            PacketManager::Get()->managerType_ = ENetworkHandleType::Client;
            serverConnectionTimePoint_ = std::chrono::steady_clock::now();
            
            bIsRunning_ = true;
        }
        #endif
    }
}

void NetHandler::DisconnectFromServer()
{
    if (bIsRunning_)
    {
        StopAndCleanUpConnection();
    }
}

bool NetHandler::InitializeWin32(const NetSettings& InSettings)
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
    bHasParentServer_ = InSettings.parentServerPort != 0;
    if (bHasParentServer_)
    {
        connectedParentServerAddress_.sin_port = htons(InSettings.parentServerPort);
        connectedParentServerAddress_.sin_family = AF_INET;
        
        if (InetPton(AF_INET, StringUtility::StringToWideString(InSettings.parentServerAddress).c_str(), &connectedParentServerAddress_.sin_addr.s_addr) != 1)
        {
            std::cerr << "Parent server address and port was invalid!" << '\n';
            bHasParentServer_ = false;
        }
    }

    // Set own address
    if (bIsServer_)
    {
        address_.sin_port = htons(InSettings.serverPort);
        address_.sin_family = AF_INET;

        std::cout << "Server Address " << InSettings.serverAddress << " : " << InSettings.serverPort << "\n";
        
        if (InetPton(AF_INET, StringUtility::StringToWideString(InSettings.serverAddress).c_str(), &address_.sin_addr.s_addr) != 1)
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
    
    return true;
}

void NetHandler::SendConnectionPacket(const VariableDataObject<CONNECTION_COMPONENT_DATA_SIZE>& InVariableData)
{
    if (!bIsServer_)
    {
        parentConnection_ = NetUtility::RetrieveStorageFromIPv4Address(connectedParentServerAddress_);
        connectionHandler_.AddConnection(parentConnection_);

        // Add server target data
        PacketManager::Get()->packetTargetDataMap_.insert({ parentConnection_, PacketTargetData() });

        // Send packet component
        ServerConnectPacketComponent connectComponent;
        connectComponent.SetVariableData(InVariableData);
        PacketManager::Get()->SendPacketComponent<ServerConnectPacketComponent>(connectComponent, parentConnection_);

        std::cout << "Connecting to server...\n";
    }
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
        }
        else if (bytesReceived == 0)
        {
            InNetHandler->KickNetTarget(senderAddress, static_cast<uint8_t>(ENetDisconnectType::ConnectionClosed));
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
    }
}

void NetHandler::SendReturnAckBackToNetTarget(const sockaddr_storage& Target, const Packet& InPacket)
{
    ReturnAckComponent component;
    component.ackIdentifier = InPacket.GetIdentifier();
    component.packetComponentType = static_cast<uint16_t>(InPacket.GetPacketType());
    PacketManager::Get()->SendPacketComponent(component, Target);
}

void NetHandler::ProcessPackets()
{
    // Retrieve and clear packets waiting for processing
    packetProcessingMutexLock_.lock();
    const std::vector<PacketProcessData> packetProcessDataCopy = packetDataToProcess_;
    packetDataToProcess_.clear();
    packetProcessingMutexLock_.unlock();

    if (!IsServer() && !packetProcessDataCopy.empty())
    {
        serverConnectionTimePoint_ = std::chrono::steady_clock::now();
    }
    
    // Process packets
    for (const PacketProcessData& data : packetProcessDataCopy)
    {
        const Packet& packet = data.packet;
        const sockaddr_storage& senderAddress = data.address;

        UpdateNetTarget(senderAddress);

        const bool bIsConnected = IsConnected(senderAddress);

        // Check if checksum of packet is valid with the received packet
        if (packet.GetCheckSum() != packet.CalculateCheckSum())
        {
            continue;
        }
        
        // Send back response if of Ack type
        if (packet.GetPacketType() == EPacketHandlingType::Ack && bIsConnected)
        {
            SendReturnAck(senderAddress, packet);
        }
        // Check if packet has already been received
        if (connectionHandler_.HasPacketBeenReceived(senderAddress, packet.GetIdentifier()))
        {
            continue;
        }

        //std::cout << packet.GetIdentifier() << " : " << (packet.GetPacketType() == EPacketHandlingType::Ack ? "Ack" : "Not Ack") << " : " << "Got Packet!\n"; // Temporary Debug
        
        // Handle Components
        std::vector<const PacketComponent*> outComponents;
        packet.GetComponents(outComponents);

        // If not connected, check if has connect component and process first
        if (!bIsConnected)
        {
            bool bFoundConnectComponent = false;
            int iter = 0;
            for (const PacketComponent* component : outComponents)
            {
                if (component->GetIdentifier() == static_cast<uint16_t>(EPacketComponent::ServerConnect))
                {
                    PacketManager::Get()->HandleComponent(senderAddress, *component);
                    outComponents.erase(outComponents.begin() + iter);
                    bFoundConnectComponent = true;
                    break;
                }
                ++iter;
            }

            // Return if not connecting
            if (!bFoundConnectComponent)
            {
                return;
            }
        }
        
        // Handle Components
        for (const PacketComponent* component : outComponents)
        {
            // Tracking packets
            if (bShouldTrackPackets_ && !IsServer())
            {
                int& count = packetComponentsReceivedCounterByType_[component->GetIdentifier()];
                ++count;
            }

            // Handle and delete
            PacketManager::Get()->HandleComponent(senderAddress, *component);
            delete[] reinterpret_cast<const uint8_t*>(component);
        }
        
        // Mark packet as received
        if (IsConnected(senderAddress))
        {
            connectionHandler_.SetPacketMarketAsReceived(senderAddress, packet.GetIdentifier());
        }
    }
}

void NetHandler::SendReturnAck(const sockaddr_storage& SenderAddress, const Packet& InPacket)
{
    SendReturnAckBackToNetTarget(SenderAddress, InPacket);
}

void NetHandler::PreProcessPackets(const char* Buffer, const int BytesReceived, const sockaddr_storage& SenderAddress)
{
    const Packet packet = { Buffer, BytesReceived };

    packetProcessingMutexLock_.lock();
    packetDataToProcess_.push_back({ SenderAddress, packet });
    packetProcessingMutexLock_.unlock();
}

void NetHandler::UpdateNetTarget(const sockaddr_storage& InAddress)
{
    connectionHandler_.UpdateNetTargetClock(InAddress);
}

void NetHandler::OnChildDisconnectReceived(const sockaddr_storage& InSender, const PacketComponent& InComponent)
{
    if (connectionHandler_.ContainsConnection(InSender))
    {
        KickNetTarget(InSender, static_cast<uint8_t>(ENetDisconnectType::Disconnected));
    }
}

void NetHandler::OnChildConnectionReceived(const sockaddr_storage& InNetTarget, const PacketComponent& InComponent)
{
    const ServerConnectPacketComponent* component = reinterpret_cast<const ServerConnectPacketComponent*>(&InComponent);
    
    if (InNetTarget.ss_family == AF_INET)
    {
        const sockaddr_in ipv4Address = NetUtility::RetrieveIPv4AddressFromStorage(InNetTarget);
        char ipString[INET_ADDRSTRLEN];
        if (inet_ntop(AF_INET, &ipv4Address.sin_addr, ipString, INET_ADDRSTRLEN))
        {
            std::cout << ipString << ":" << ntohs(ipv4Address.sin_port) << " connected to the server!\n";    
        }
    }
    
    if (!IsConnected(InNetTarget))
    {
        connectionHandler_.AddConnection(InNetTarget);
        PacketManager::Get()->OnNetTargetConnected(InNetTarget);

        // Send Client connection success packet component back
        SuccessfullyConnectedToServer successPacketComponent;
        successPacketComponent.SetVariableData(component->variableDataObject);
        PacketManager::Get()->SendPacketComponent(successPacketComponent, InNetTarget);
    }
}

void NetHandler::OnConnectionToServerSuccessful(const sockaddr_storage& InSender, const PacketComponent& InComponent)
{
    std::cout << "Successfully connected to server!\n";
    EventSystem::Get()->onConnectedToServerEvent.Execute(InSender);
}

void NetHandler::KickInactiveNetTargets()
{
    using namespace std::chrono;
    
    constexpr int iterationAddition = static_cast<int>(1.f / NET_INACTIVE_CHECKS_PER_UPDATE_PERCENT);
    
    static int iterationOffset = 0;

    std::vector<sockaddr_storage> clientsToKickByTimeOut;
    const std::unordered_map<sockaddr_in, NetTarget>& childConnections = connectionHandler_.GetConnections();
    for (const auto& connection : childConnections)
    {
        const NetTarget& netTarget = connection.second;
        
        const steady_clock::time_point currentTime = steady_clock::now();
        const duration<float> timeDifference = duration_cast<duration<float>>(currentTime - netTarget.lastTimeReceivedNetEvent);
        
        if (timeDifference.count() > NET_TIME_UNTIL_TIME_OUT_SEC)
        {
            clientsToKickByTimeOut.push_back(netTarget.address);
        }
    }

    // Kick Timed out Clients
    for (const sockaddr_storage& client : clientsToKickByTimeOut)
    {
        KickNetTarget(client, static_cast<uint8_t>(ENetDisconnectType::TimeOut));
    }
    
    ++iterationOffset;
    if (iterationOffset == iterationAddition)
    {
        iterationOffset = 0;
    }
}

void NetHandler::KickNetTarget(const sockaddr_storage& InAddress, const uint8_t InKickReason)
{
    if (connectionHandler_.ContainsConnection(InAddress))
    {
        // Send kick packet to client
        KickedFromServerPacketComponent kickedFromServerPacketComponent;
        kickedFromServerPacketComponent.disconnectType = InKickReason;
        
        Packet packet(EPacketHandlingType::None);
        packet.AddComponent(kickedFromServerPacketComponent);
        packet.CalculateAndUpdateCheckSum();
        
        SendPacketToTarget(InAddress, packet);

        // Remove tracked net target
        connectionHandler_.RemoveConnection(InAddress);
        PacketManager::Get()->OnNetTargetDisconnection(InAddress, InKickReason);

        if (InAddress.ss_family == AF_INET)
        {
            const sockaddr_in ipv4Address = NetUtility::RetrieveIPv4AddressFromStorage(InAddress);
            char ipString[INET_ADDRSTRLEN];
            if (inet_ntop(AF_INET, &ipv4Address.sin_addr, ipString, INET_ADDRSTRLEN))
            {
                std::cout << ipString << ":" << ntohs(ipv4Address.sin_port) << " kicked/disconnected from the server! " << InKickReason << "\n";    
            }
        }
    }
}

void NetHandler::OnKickedFromServerReceived(const sockaddr_storage& InAddress, const PacketComponent& InComponent)
{
    bShouldDisconnect_ = true;
}

void NetHandler::StopAndCleanUpConnection()
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

    packetDataToProcess_.clear();

    connectionHandler_ = NetConnectionHandler();
    
    PacketManager::End();
    PacketManager::Initialize();

    EventSystem::Get()->onCloseConnectionEvent.Execute();
}
}
