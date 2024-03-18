#pragma once

#include "PacketComponentTypes.hpp"
#include "Packet/PacketComponent.h"

class SpawnEntityComponent : public Net::PacketComponent
{
public:
	SpawnEntityComponent();

	uint16_t entityId;
	uint64_t entityTypeHash;

	float xPos = 0.f;
	float yPos = 0.f;

	float direction = 0.f;
};

inline SpawnEntityComponent::SpawnEntityComponent()
    : PacketComponent(static_cast<int16_t>(EPacketComponentType::SpawnEntity), sizeof(SpawnEntityComponent))
{ }