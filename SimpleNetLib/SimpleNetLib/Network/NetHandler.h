#pragma once

#include "../NetIncludes.h"

class ServerConnectPacketComponent;
class ServerDisconnectPacketComponent;
class Packet;

namespace std
{
    class thread;
}

class NetHandler
{
public:
    explicit NetHandler(const NetSettings& InNetSettings);
    ~NetHandler();

    bool IsServer() const { return bIsServer_; }

    void SendPacketToTarget(const NetTarget& InTarget, const Packet& InPacket);
    
    bool RetrieveChildConnectionNetTargetInstance(const sockaddr_storage& InAddress, NetTarget*& OutNetTarget);
    bool IsConnected(const sockaddr_storage& InAddress);
    
private:
    bool InitializeWin32();

    static void PacketListener(NetHandler* InNetHandler);
    
    void ProcessPackets(const char* Buffer, const int BytesReceived);

    void UpdateNetTarget(const sockaddr_storage& InAddress);

    void OnChildDisconnectReceived(const NetTarget& InNetTarget, const ServerDisconnectPacketComponent& InComponent);
    void OnChildConnectionReceived(const NetTarget& InNetTarget, const ServerConnectPacketComponent& InComponent);
    
    void KickInactiveNetTargets();

    void KickNetTarget(const sockaddr_storage& InAddress, const ENetDisconnectType InKickReason);
    
    WSADATA wsaData_;
    SOCKET udpSocket_;

    // Can be used by server as proxy connection
    sockaddr_in connectedParentServerAddress_;
    sockaddr_in address_;

    NetTarget parentConnection_;
    std::vector<NetTarget> childConnections_;
    
    NetSettings netSettings_;

    std::thread* packetListenerThread_ = nullptr;
    
    bool bHasParentServer_ = false;
    const bool bIsServer_ = false;

    bool bIsRunning_ = true;

    friend class PacketManager;
};
