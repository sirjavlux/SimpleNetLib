#pragma once

#include "PacketComponentTypes.hpp"
#include "Packet/PacketComponent.h"

class SpawnEntityComponent : public Net::PacketComponent
{
public:
  SpawnEntityComponent();

  uint16_t entityId;
  uint64_t entityTypeHash;
};

inline SpawnEntityComponent::SpawnEntityComponent()
    : PacketComponent(static_cast<int16_t>(EPacketComponentType::SpawnEntity), sizeof(SpawnEntityComponent))
{ }