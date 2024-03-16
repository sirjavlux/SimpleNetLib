#pragma once

#include "Entity.h"

class ColliderComponent;

class PlayerShipEntity : public Entity
{
public:
	~PlayerShipEntity();
	
	void Init() override;
	void InitComponents() override;
	
	void Update(float InDeltaTime) override;
	void FixedUpdate() override;
	
	void OnTriggerEntered(const ColliderComponent& InCollider);
	void OnTriggerExit(const ColliderComponent& InCollider);

	void OnEntityDeath(Entity& InKiller);
	
	void OnReadReplication(const DataReplicationPacketComponent& InComponent) override;
	void OnSendReplication(DataReplicationPacketComponent& OutComponent) override;

	void SetUsername(const std::string& InUsername) { username_ = InUsername; }
	const NetTag& GetUsername() const { return username_; }
	
private:
	NetTag username_;
};
