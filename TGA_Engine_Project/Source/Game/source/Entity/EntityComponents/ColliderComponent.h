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
  
	void Update(float InDeltaTime) override;

	void Remove() { bIsMarkedForRemoval_ = true; }
	bool IsMarkedForRemoval() const { return bIsMarkedForRemoval_; }

	void SetCollider(const std::shared_ptr<Collider>& InCollider);

	Collider* GetCollider() { return collider_.get(); }

	void OnColliderCollision(const ColliderComponent& InColliderComponent);
	
	DynamicMulticastDelegate<const ColliderComponent&> collisionDelegate;
	
private:
	bool bIsMarkedForRemoval_ = false;

	std::shared_ptr<Collider> collider_;
};
