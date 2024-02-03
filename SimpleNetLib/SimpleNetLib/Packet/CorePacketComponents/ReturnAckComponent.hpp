#pragma once

#include "../PacketComponent.h"

class NET_LIB_EXPORT ReturnAckComponent : public PacketComponent
{
public:
    explicit ReturnAckComponent();

    int32_t ackIdentifier;
};

inline ReturnAckComponent::ReturnAckComponent()
    : PacketComponent(static_cast<int16_t>(EPacketComponent::ReturnAck), sizeof(ReturnAckComponent)),
    ackIdentifier(-1)
{ }
