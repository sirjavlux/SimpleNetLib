#pragma once

#include "PacketComponentTypes.hpp"
#include "PositionUpdateData.hpp"
#include "Packet/PacketComponent.h"

// TODO: Optimize this
class SpawnBulletComponent : public Net::PacketComponent
{
public:
	SpawnBulletComponent();

	uint16_t entityId;
	uint16_t shooterEntityId;
		
	PositionUpdateData positionUpdateData;
};

inline SpawnBulletComponent::SpawnBulletComponent()
    : PacketComponent(static_cast<int16_t>(EPacketComponentType::SpawnBullet), sizeof(SpawnBulletComponent))
{ }