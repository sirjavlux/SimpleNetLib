#pragma once

#include "PacketComponentTypes.hpp"
#include "Packet/PacketComponent.h"

class UpdateStatComponent : public Net::PacketComponent
{
public:
	UpdateStatComponent();

	uint64_t statTagHash = 0;
	float stat = 0.f;
	uint16_t entityIdentifier = 0;
};

inline UpdateStatComponent::UpdateStatComponent()
	: PacketComponent(static_cast<int16_t>(EPacketComponentType::UpdateStat), sizeof(UpdateStatComponent))
{ }