#pragma once

#include "NetStructs.hpp"


class NetHandler
{
public:
    explicit NetHandler(const NetSettings& InNetSettings);
    ~NetHandler();
    
private:
    void Initialize();

    WSADATA wsaData_;
    SOCKET udpSocket_;

    // Can be used by server as proxy connection
    sockaddr_in connectedParentServerAddress_;
    sockaddr_in address_;

    NetSettings netSettings_;
};
