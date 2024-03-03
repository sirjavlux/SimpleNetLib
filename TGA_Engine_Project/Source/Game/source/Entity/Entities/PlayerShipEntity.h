#pragma once

#include "Entity.hpp"

class ColliderComponent;

class PlayerShipEntity : public Entity
{
public:
	void Init() override;
	
	void Update(float InDeltaTime) override;

	void OnTriggerEntered(const ColliderComponent& InCollider);
	void OnTriggerExit(const ColliderComponent& InCollider);

	uint16_t GetHealth() const { return health_; }
	uint16_t GetMaxHealth() const { return maxHealth_; }

	void OnSendReplication(DataReplicationPacketComponent& OutComponent) override;
	void OnReadReplication(DataReplicationPacketComponent& InComponent) override;
	
private:
	uint16_t health_ = 0;
	uint16_t maxHealth_ = 100;
};
