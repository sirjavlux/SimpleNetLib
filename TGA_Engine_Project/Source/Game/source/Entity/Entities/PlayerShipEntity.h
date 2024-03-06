﻿#pragma once

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

	void OnReadReplication(const DataReplicationPacketComponent& InComponent) override;
	void OnSendReplication(DataReplicationPacketComponent& OutComponent) override;

	void SetUsername(const std::string& InUsername) { username_ = InUsername; }
	const NetTag& GetUsername() const { return username_; }
	
private:
	NetTag username_;
	
	uint16_t health_ = 0;
	uint16_t maxHealth_ = 100;
};
