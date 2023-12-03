#pragma once

#include "../PacketComponent.h"

class NET_LIB_EXPORT KickedFromServerPacketComponent : public PacketComponent
{
public:
    KickedFromServerPacketComponent(const ENetDisconnectType InDisconnectType);

    ENetDisconnectType disconnectType;
};

inline KickedFromServerPacketComponent::KickedFromServerPacketComponent(const ENetDisconnectType InDisconnectType)
    : PacketComponent(static_cast<int16_t>(EPacketComponent::KickedFromServer), sizeof(KickedFromServerPacketComponent)),
    disconnectType(InDisconnectType)
{ }
