#pragma once

#include "EntityComponent.hpp"
#include "Utility/NetVector3.h"
#include "NetIncludes.h"
#include "tge/math/Vector2.h"

struct Collider;
class ColliderComponent : public EntityComponent
{
public:
	void Init() override; // TODO: Add collider to collision manager somehow
  
	void FixedUpdate() override;

	void Remove() { bIsMarkedForRemoval_ = true; }
	bool IsMarkedForRemoval() const { return bIsMarkedForRemoval_; }

	void SetCollider(const std::shared_ptr<Collider>& InCollider);

	Collider* GetCollider() { return collider_.get(); }

	void SetIsTrigger(const bool InShouldBeTrigger) { bIsTrigger_ = InShouldBeTrigger; }
	
	void OnTriggerEnter(const ColliderComponent& InColliderComponent);
	void OnTriggerExit(const ColliderComponent& InColliderComponent);
	void OnColliderCollision(const ColliderComponent& InColliderComponent);

	const Tga::Vector2f& GetPositionLastFrame() const { return positionLastFrame_; }
	
	DynamicMulticastDelegate<const ColliderComponent&> triggerEnterDelegate;
	DynamicMulticastDelegate<const ColliderComponent&> triggerExitDelegate;
	DynamicMulticastDelegate<const ColliderComponent&> collisionDelegate;
	
private:
	Tga::Vector2f positionLastFrame_;
	
	bool bIsMarkedForRemoval_ = false;

	bool bIsTrigger_ = true;
	
	std::unordered_map<uint16_t, ColliderComponent> collidersEntered_;
	std::unordered_map<uint16_t, ColliderComponent> collidersCollidedWithLastUpdate_;
	
	std::shared_ptr<Collider> collider_;
};
