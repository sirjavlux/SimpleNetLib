#pragma once

#include "NetConnectionHandler.h"
#include "../NetIncludes.h"
#include "../Packet/Packet.h"

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
    explicit NetHandler(const NetSettings& InNetSettings);
    ~NetHandler();

    void Update();
    
    void Initialize();
    
    bool IsServer() const { return bIsServer_; }

    void SendPacketToTargetAndResetPacket(const sockaddr_storage& InTarget, Packet& InPacket) const;
    void SendPacketToTarget(const sockaddr_storage& InTarget, const Packet& InPacket) const;
    
    const NetTarget* RetrieveChildConnectionNetTargetInstance(const sockaddr_storage& InAddress) const;
    bool IsConnected(const sockaddr_storage& InAddress);

    NetConnectionHandler& GetNetConnectionHandler() { return connectionHandler_; }
    
private:
    
    bool InitializeWin32();

    static void SendReturnAckBackToNetTarget(const sockaddr_storage& Target, uint32_t Identifier);

    void ProcessPackets();
    
    static void PacketListener(NetHandler* InNetHandler);

    static void SendReturnAck(const sockaddr_storage& SenderAddress, uint32_t Identifier);
    
    void PreProcessPackets(const char* Buffer, int BytesReceived, const sockaddr_storage& SenderAddress);

    void UpdateNetTarget(const sockaddr_storage& InAddress);

    void OnChildDisconnectReceived(const sockaddr_storage& InSender, const PacketComponent& InComponent);
    void OnChildConnectionReceived(const sockaddr_storage& InNetTarget, const PacketComponent& InComponent);
    void OnConnectionToServerSuccessful(const sockaddr_storage& InNetTarget, const PacketComponent& InComponent);
    
    void KickInactiveNetTargets();

    void KickNetTarget(const sockaddr_storage& InAddress, ENetDisconnectType InKickReason);
    
    WSADATA wsaData_;
    SOCKET udpSocket_;

    // Can be used by server as proxy connection
    sockaddr_in connectedParentServerAddress_;
    sockaddr_in address_;

    sockaddr_storage parentConnection_;
    NetConnectionHandler connectionHandler_;
    
    NetSettings netSettings_;

    std::thread* packetListenerThread_ = nullptr;

    std::mutex packetProcessingMutexLock_;
    std::vector<PacketProcessData> packetDataToProcess_;
    
    bool bHasParentServer_ = false;
    const bool bIsServer_ = false;

    bool bIsRunning_ = true;
    
    friend class PacketManager;
};
}