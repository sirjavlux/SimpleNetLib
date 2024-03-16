#pragma once

#include <mutex>

#include "NetConnectionHandler.h"
#include "../Packet/Packet.h"
#include "../Utility/VariableDataObject.hpp"

class ServerConnectPacketComponent;
class ServerDisconnectPacketComponent;

namespace Net
{
    class PacketComponent;
    class Packet;
}

namespace std
{
    class thread;
}

struct PacketProcessData
{
    sockaddr_storage address;
    Net::Packet packet;
};

namespace Net
{
class NetHandler
{
public:
    explicit NetHandler();
    ~NetHandler();

    void Initialize();
    
    bool IsServer() const { return bIsServer_; }

    void SetUpServer(PCSTR InServerAddress, u_short InServerPort);
    void ConnectToServer(const VariableDataObject<CONNECTION_COMPONENT_DATA_SIZE>& InVariableData, PCSTR InServerAddress, u_short InServerPort);
    void DisconnectFromServer();
    
    void SendPacketToTargetAndResetPacket(const sockaddr_storage& InTarget, Packet& InPacket) const;
    void SendPacketToTarget(const sockaddr_storage& InTarget, const Packet& InPacket) const;
    
    const NetTarget* RetrieveChildConnectionNetTargetInstance(const sockaddr_storage& InAddress) const;
    bool IsConnected(const sockaddr_storage& InAddress);

    bool IsRunning() const { return bIsRunning_; }
    
    NetConnectionHandler& GetNetConnectionHandler() { return connectionHandler_; }

    const sockaddr_storage& GetParentConnection() const { return parentConnection_; }

    void OnChildDisconnectReceived(const sockaddr_storage& InSender, const PacketComponent& InComponent);
    void OnChildConnectionReceived(const sockaddr_storage& InNetTarget, const PacketComponent& InComponent);
    void OnConnectionToServerSuccessful(const sockaddr_storage& InNetTarget, const PacketComponent& InComponent);

    void KickNetTarget(const sockaddr_storage& InAddress, uint8_t InKickReason);

    void OnKickedFromServerReceived(const sockaddr_storage& InAddress, const PacketComponent& InComponent);
    
    const sockaddr_in& GetAddress() const { return address_; }
    
private:
    void Update();
    
    bool InitializeWin32(const NetSettings& InSettings);
    void SendConnectionPacket(const VariableDataObject<CONNECTION_COMPONENT_DATA_SIZE>& InVariableData);
    
    static void SendReturnAckBackToNetTarget(const sockaddr_storage& Target, const Packet& InPacket);

    void ProcessPackets();
    
    static void PacketListener(NetHandler* InNetHandler);

    static void SendReturnAck(const sockaddr_storage& SenderAddress, const Packet& InPacket);
    
    void PreProcessPackets(const char* Buffer, int BytesReceived, const sockaddr_storage& SenderAddress);

    void UpdateNetTarget(const sockaddr_storage& InAddress);
    
    void KickInactiveNetTargets();

    void StopAndCleanUpConnection();
    
    WSADATA wsaData_;
    SOCKET udpSocket_;

    // Can be used by server as proxy connection
    sockaddr_in connectedParentServerAddress_;
    sockaddr_in address_;

    sockaddr_storage parentConnection_;
    NetConnectionHandler connectionHandler_;

    std::thread* packetListenerThread_ = nullptr;

    std::mutex packetProcessingMutexLock_;
    std::vector<PacketProcessData> packetDataToProcess_;
    
    std::chrono::steady_clock::time_point serverConnectionTimePoint_;
    
    bool bHasParentServer_ = false;
    bool bIsServer_ = false;

    bool bIsRunning_ = false;

    bool bShouldDisconnect_ = false;
    
    friend class SimpleNetLibCore;
};
}