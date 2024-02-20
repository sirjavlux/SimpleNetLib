#pragma once

#include "../PacketComponent.h"

class NET_LIB_EXPORT ServerDisconnectPacketComponent : public Net::PacketComponent
{
public:
    ServerDisconnectPacketComponent();
    
};

inline ServerDisconnectPacketComponent::ServerDisconnectPacketComponent()
    : PacketComponent(static_cast<int16_t>(Net::EPacketComponent::ServerDisconnect), sizeof(ServerDisconnectPacketComponent))
{ }