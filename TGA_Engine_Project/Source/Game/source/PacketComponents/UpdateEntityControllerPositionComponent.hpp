#pragma once

#include "PacketComponentTypes.hpp"
#include "../Entity/EntityComponents/ControllerComponent.h"
#include "Packet/PacketComponent.h"

class UpdateEntityControllerPositionComponent : public Net::PacketComponent
{
public:
	UpdateEntityControllerPositionComponent();

	uint16_t entityIdentifier = 0;
	
	PositionUpdateEntry positionUpdateEntry;
};

inline UpdateEntityControllerPositionComponent::UpdateEntityControllerPositionComponent()
    : PacketComponent(static_cast<int16_t>(EPacketComponentType::UpdateEntityControllerPosition), sizeof(UpdateEntityControllerPositionComponent))
{ }