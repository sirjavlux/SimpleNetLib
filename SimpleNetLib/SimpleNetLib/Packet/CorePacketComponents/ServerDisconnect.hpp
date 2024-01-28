#pragma once

#include "../PacketComponent.h"

class NET_LIB_EXPORT ServerDisconnectPacketComponent : public PacketComponent
{
public:
    ServerDisconnectPacketComponent();
    
};

inline ServerDisconnectPacketComponent::ServerDisconnectPacketComponent()
    : PacketComponent(static_cast<int16_t>(EPacketComponent::ServerDisconnect), sizeof(ServerDisconnectPacketComponent))
{ }