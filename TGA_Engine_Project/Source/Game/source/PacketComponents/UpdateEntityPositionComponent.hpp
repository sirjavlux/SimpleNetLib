#pragma once

#include "PacketComponentTypes.hpp"
#include "PositionUpdateData.hpp"
#include "Packet/PacketComponent.h"

class UpdateEntityPositionComponent : public Net::PacketComponent
{
public:
	UpdateEntityPositionComponent();
	
	uint16_t entityIdentifier = 0;

	PositionUpdateData positionUpdateData;
};

inline UpdateEntityPositionComponent::UpdateEntityPositionComponent()
    : PacketComponent(static_cast<int16_t>(EPacketComponentType::UpdateEntityPosition), sizeof(UpdateEntityPositionComponent))
{ }