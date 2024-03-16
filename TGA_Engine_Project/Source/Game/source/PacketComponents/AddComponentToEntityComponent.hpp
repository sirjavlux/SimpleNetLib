#pragma once

#include "PacketComponentTypes.hpp"
#include "Packet/PacketComponent.h"

class AddComponentToEntityComponent : public Net::PacketComponent
{
public:
	AddComponentToEntityComponent();

	uint16_t entityId;
	uint16_t entityComponentId;
	uint64_t typeHash;
};

inline AddComponentToEntityComponent::AddComponentToEntityComponent()
    : PacketComponent(static_cast<int16_t>(EPacketComponentType::AddEntityComponent), sizeof(AddComponentToEntityComponent))
{ }