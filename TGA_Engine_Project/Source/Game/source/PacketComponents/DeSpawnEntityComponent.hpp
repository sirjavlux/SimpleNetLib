#pragma once

#include "PacketComponentTypes.hpp"
#include "Packet/PacketComponent.h"

class DeSpawnEntityComponent : public Net::PacketComponent
{
public:
  DeSpawnEntityComponent();

  uint16_t entityId;
};

inline DeSpawnEntityComponent::DeSpawnEntityComponent()
    : PacketComponent(static_cast<int16_t>(EPacketComponentType::DeSpawnEntity), sizeof(DeSpawnEntityComponent))
{ }