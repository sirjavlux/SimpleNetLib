#pragma once

#include "Entity.h"
#include "../AsteroidManager.h"

class ColliderComponent;

class AsteroidEntity : public Entity
{
public:
	void Init() override;
	void InitComponents() override;

	void OnSpawnHasBeenReceived() override;
	
	void Update(float InDeltaTime) override;
	void FixedUpdate() override;
	
	void OnTriggerEntered(const ColliderComponent& InCollider);
	void OnEntityDeath(uint16_t InEnemy);
	
	void SetGenerationData(const GenerationData& InGenerationData) { generationData_ = InGenerationData; }
	
private:
	GenerationData generationData_;

	float travelSpeed = 0.f;
};
