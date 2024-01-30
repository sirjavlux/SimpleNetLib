#pragma once

#include "../PacketComponent.h"

class NET_LIB_EXPORT KickedFromServerPacketComponent : public PacketComponent
{
public:
    explicit KickedFromServerPacketComponent();

    ENetDisconnectType disconnectType;
};

inline KickedFromServerPacketComponent::KickedFromServerPacketComponent()
    : PacketComponent(static_cast<int16_t>(EPacketComponent::KickedFromServer), sizeof(KickedFromServerPacketComponent)),
    disconnectType()
{}
