#pragma once

#include "../PacketComponent.h"

class NET_LIB_EXPORT ServerPingPacketComponent : public PacketComponent
{
public:
    ServerPingPacketComponent();
    
};

inline ServerPingPacketComponent::ServerPingPacketComponent()
    : PacketComponent(static_cast<int16_t>(EPacketComponent::ServerPing), sizeof(ServerPingPacketComponent))
{ }