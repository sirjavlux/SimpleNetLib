#pragma once

#include "PacketComponentTypes.hpp"
#include "Packet/PacketComponent.h"

class SpawnBulletComponent : public Net::PacketComponent
{
public:
	SpawnBulletComponent();

	uint16_t entityId;
	uint16_t shooterEntityId;
		
	float xPos = 0.f;
	float yPos = 0.f;
	
	float xDir = 0.f;
    float yDir = 0.f;
};

inline SpawnBulletComponent::SpawnBulletComponent()
    : PacketComponent(static_cast<int16_t>(EPacketComponentType::SpawnBullet), sizeof(SpawnBulletComponent))
{ }