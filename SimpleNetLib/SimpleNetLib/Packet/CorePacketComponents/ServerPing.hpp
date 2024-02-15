#pragma once

#include "../PacketComponent.h"

class NET_LIB_EXPORT ServerPingPacketComponent : public Net::PacketComponent
{
public:
    ServerPingPacketComponent();

    NetUtility::NetPosition location;
};

inline ServerPingPacketComponent::ServerPingPacketComponent()
    : PacketComponent(static_cast<int16_t>(Net::EPacketComponent::ServerPing), sizeof(ServerPingPacketComponent))
{ }