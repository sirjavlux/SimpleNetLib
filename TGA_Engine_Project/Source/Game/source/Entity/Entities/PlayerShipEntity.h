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
};
