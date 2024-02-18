#pragma once

#include "PacketComponentTypes.hpp"
#include "Packet/PacketComponent.h"

class DeSpawnEntityComponent : public Net::PacketComponent
{
public:
  DeSpawnEntityComponent();
};

inline DeSpawnEntityComponent::DeSpawnEntityComponent()
    : PacketComponent(static_cast<int16_t>(EPacketComponentType::DeSpawnEntity), sizeof(DeSpawnEntityComponent))
{ }