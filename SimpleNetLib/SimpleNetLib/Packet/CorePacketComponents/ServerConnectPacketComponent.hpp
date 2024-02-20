#pragma once

#include "../PacketComponent.h"

class NET_LIB_EXPORT ServerConnectPacketComponent : public Net::PacketComponent
{
public:
    ServerConnectPacketComponent();
    
};

inline ServerConnectPacketComponent::ServerConnectPacketComponent()
    : PacketComponent(static_cast<int16_t>(Net::EPacketComponent::ServerConnect), sizeof(ServerConnectPacketComponent))
{ }
