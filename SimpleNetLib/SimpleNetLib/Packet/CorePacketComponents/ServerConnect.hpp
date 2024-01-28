#pragma once

#include "../PacketComponent.h"

class NET_LIB_EXPORT ServerConnectPacketComponent : public PacketComponent
{
public:
    ServerConnectPacketComponent();
    
};

inline ServerConnectPacketComponent::ServerConnectPacketComponent()
    : PacketComponent(static_cast<int16_t>(EPacketComponent::ServerConnect), sizeof(ServerConnectPacketComponent))
{ }
