﻿#pragma once

#include "NetIncludes.h"
#include "Packet/PacketComponent.h"

namespace Net
{
class PacketManager;
}
namespace Net
{
class PacketComponentRegistry;
class PacketComponentDelegator;
class NetHandler;

class SimpleNetLibCore
{
public:
    static SimpleNetLibCore* Initialize();
    static SimpleNetLibCore* Get();
    static void End();

    void Update();
    
    SimpleNetLibCore();
    ~SimpleNetLibCore();

    void SetUpServer(PCWSTR InServerAddress = DEFAULT_SERVER_ADDRESS_WIDE, u_short InServerPort = DEFAULT_SERVER_PORT);
    void ConnectToServer(PCWSTR InServerAddress = DEFAULT_SERVER_ADDRESS_WIDE, u_short InServerPort = DEFAULT_SERVER_PORT);

    void DisconnectFromServer();

    NetHandler* GetNetHandler() { return netHandler_; }
    PacketComponentDelegator* GetPacketComponentDelegator() { return packetComponentHandleDelegator_; }
    PacketComponentRegistry* GetPacketComponentRegistry() { return packetComponentRegistry_; }

    static PacketManager* GetPacketManager();
    
private:
    static SimpleNetLibCore* instance_;
    NetHandler* netHandler_ = nullptr;
    PacketComponentDelegator* packetComponentHandleDelegator_ = nullptr;
    PacketComponentRegistry* packetComponentRegistry_ = nullptr;
};
}
