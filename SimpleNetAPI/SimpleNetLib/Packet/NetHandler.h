#pragma once

#include "NetStructs.hpp"


class NetHandler
{
public:
    explicit NetHandler(const NetSettings& InNetSettings);
    ~NetHandler();

    bool IsServer() const { return bIsServer_; }
    
private:
    void InitializeWin32();

    WSADATA wsaData_;
    SOCKET udpSocket_;

    // Can be used by server as proxy connection
    sockaddr_in connectedParentServerAddress_;
    sockaddr_in address_;

    NetSettings netSettings_;

    bool bHasParentServer_ = false;
    const bool bIsServer_ = false;
};
