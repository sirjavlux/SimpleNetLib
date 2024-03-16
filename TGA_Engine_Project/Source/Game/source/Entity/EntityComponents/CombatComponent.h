#pragma once

#include "EntityComponent.h"
#include "NetIncludes.h"

class ColliderComponent;
class CombatComponent : public EntityComponent
{
public:
	void Init() override;
  
	void FixedUpdate() override;
	
	void OnTriggerEntered(const ColliderComponent& InColliderComponent);

	void OnSendReplication(DataReplicationPacketComponent& OutComponent) override;
	void OnReadReplication(const DataReplicationPacketComponent& InComponent) override;

	void Damage(uint16_t InEnemyId, float InDamage);

	void SetCollisionDamage(const float InNewCollisionDamage) { collisionDamage_ = InNewCollisionDamage; }
	float GetCollisionDamage() const { return collisionDamage_; }

	void HealToFullHealth();
	void SetHealth(float InHealth);
	float GetHealth() const;

	void SetMaxHealth(const float InNewMaxHealth) { maxHealth_ = InNewMaxHealth; }
	float GetMaxHealth() const { return maxHealth_; }
	
	DynamicMulticastDelegate<Entity&> entityDeathDelegate;
	DynamicMulticastDelegate<Entity&, float> entityTakeDamageDelegate;
	
private:
	float maxHealth_ = 100.f;
	float collisionDamage_ = 0.f;

	std::vector<uint16_t> friendlyEntities_;
};