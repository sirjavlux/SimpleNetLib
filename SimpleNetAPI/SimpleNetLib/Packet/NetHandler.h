#pragma once

#include "NetStructs.hpp"

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
    
private:
    bool InitializeWin32();

    static void PacketListener(NetHandler* InNetHandler);
    
    void ProcessPackets(const char* buffer, int bytesReceived);
    
    WSADATA wsaData_;
    SOCKET udpSocket_;

    // Can be used by server as proxy connection
    sockaddr_in connectedParentServerAddress_;
    sockaddr_in address_;

    NetSettings netSettings_;

    std::thread* packetListenerThread_ = nullptr;
    
    bool bHasParentServer_ = false;
    const bool bIsServer_ = false;

    bool bIsRunning_ = true;
};
