#pragma once

#include "Entity.hpp"

class ColliderComponent;

class PlayerShipEntity : public Entity
{
public:
	void Init() override;
	
	void Update(float InDeltaTime) override;
	void FixedUpdate() override;
	
	void OnTriggerEntered(const ColliderComponent& InCollider);
	void OnTriggerExit(const ColliderComponent& InCollider);
	
	void OnReadReplication(const DataReplicationPacketComponent& InComponent) override;
	void OnSendReplication(DataReplicationPacketComponent& OutComponent) override;

	void SetUsername(const std::string& InUsername) { username_ = InUsername; }
	const NetTag& GetUsername() const { return username_; }

	void TakeDamage(int InDamage, uint16_t InDamagingEntity);

	uint16_t GetMaxHealth() const { return maxHealth_; }
	
private:
	NetTag username_;

	uint16_t maxHealth_ = 100;
};
