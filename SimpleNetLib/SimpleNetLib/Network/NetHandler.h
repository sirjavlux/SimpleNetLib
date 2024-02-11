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

    void SendPacketToTargetAndResetPacket(const NetTarget& InTarget, Packet& InPacket) const;
    void SendPacketToTarget(const NetTarget& InTarget, const Packet& InPacket) const;
    
    bool RetrieveChildConnectionNetTargetInstance(const sockaddr_storage& InAddress, NetTarget& OutNetTarget);
    bool IsConnected(const sockaddr_storage& InAddress);
    
private:
    
    bool InitializeWin32();

    static void SendReturnAckBackToNetTarget(const NetTarget& Target, const int32_t Identifier);

    void ProcessPackets();
    
    // Functions below runs on listen thread
    
    static void PacketListener(NetHandler* InNetHandler);
    
    bool HandleReturnAck(const sockaddr_storage& SenderAddress, const int32_t Identifier);
    void UpdatePacketTracker(const sockaddr_storage& SenderAddress, const int32_t Identifier);
    
    void PreProcessPackets(const char* Buffer, const int BytesReceived, const sockaddr_storage& SenderAddress);

    void UpdateNetTarget(const sockaddr_storage& InAddress);

    void OnChildDisconnectReceived(const NetTarget& InNetTarget, const PacketComponent& InComponent);
    void OnChildConnectionReceived(const NetTarget& InNetTarget, const PacketComponent& InComponent);
    
    void KickInactiveNetTargets();

    void KickNetTarget(const sockaddr_storage& InAddress, const ENetDisconnectType InKickReason);
    
    WSADATA wsaData_;
    SOCKET udpSocket_;

    // Can be used by server as proxy connection
    sockaddr_in connectedParentServerAddress_;
    sockaddr_in address_;

    NetTarget parentConnection_;
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