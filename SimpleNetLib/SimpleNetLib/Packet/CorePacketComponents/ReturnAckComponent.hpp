#pragma once

#include "../PacketComponent.h"

class NET_LIB_EXPORT ReturnAckComponent : public Net::PacketComponent
{
public:
    explicit ReturnAckComponent();

    uint16_t packetComponentType;
    uint32_t ackIdentifier;
};

inline ReturnAckComponent::ReturnAckComponent()
    : PacketComponent(static_cast<int16_t>(Net::EPacketComponent::ReturnAck), sizeof(ReturnAckComponent)),
    ackIdentifier(0)
{ }
