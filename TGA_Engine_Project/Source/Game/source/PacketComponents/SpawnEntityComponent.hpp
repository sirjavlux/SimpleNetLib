#pragma once

#include "PacketComponentTypes.hpp"
#include "Packet/PacketComponent.h"

class SpawnEntityComponent : public Net::PacketComponent
{
public:
  SpawnEntityComponent();
};

inline SpawnEntityComponent::SpawnEntityComponent()
    : PacketComponent(static_cast<int16_t>(EPacketComponentType::SpawnEntity), sizeof(SpawnEntityComponent))
{ }