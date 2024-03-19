#pragma once

#include "EntityComponent.h"
#include "Utility/NetVector3.h"
#include "NetIncludes.h"
#include "tge/math/Vector2.h"

enum class ECollisionFilter : uint16_t
{
	None				= 0,
	Player				= 1 << 0,
	Projectile			= 1 << 1,
	WorldDestructible	= 1 << 2,
};

inline ECollisionFilter operator|(ECollisionFilter InLhs, ECollisionFilter InRhs)
{
	return static_cast<ECollisionFilter>(static_cast<uint16_t>(InLhs) | static_cast<uint16_t>(InRhs));
}

inline ECollisionFilter operator&(ECollisionFilter InLhs, ECollisionFilter InRhs)
{
	return static_cast<ECollisionFilter>(static_cast<uint16_t>(InLhs) & static_cast<uint16_t>(InRhs));
}

inline ECollisionFilter operator|=(ECollisionFilter InLhs, ECollisionFilter InRhs)
{
	return static_cast<ECollisionFilter>(static_cast<uint16_t>(InLhs) | static_cast<uint16_t>(InRhs));
}

inline ECollisionFilter operator&=(ECollisionFilter InLhs, ECollisionFilter InRhs)
{
	return static_cast<ECollisionFilter>(static_cast<uint16_t>(InLhs) & static_cast<uint16_t>(InRhs));
}

struct Collider;
class ColliderComponent : public EntityComponent
{
public:
	void Init() override; // TODO: Add collider to collision manager somehow
	
	void FixedUpdate() override;
	
	void SetCollider(const std::shared_ptr<Collider>& InCollider);

	Collider* GetCollider() { return collider_.get(); }

	void SetIsTrigger(const bool InShouldBeTrigger) { bIsTrigger_ = InShouldBeTrigger; }
	
	void OnTriggerEnter(const ColliderComponent& InColliderComponent);
	void OnTriggerExit(const ColliderComponent& InColliderComponent);
	void OnColliderCollision(ColliderComponent* InColliderComponent);

	const Tga::Vector2f& GetPositionLastFrame() const { return positionLastFrame_; }

	ECollisionFilter GetCollisionFilter() const { return collisionFilter_; }
	void SetCollisionFilter(const ECollisionFilter InCollisionFilter) { collisionFilter_ = InCollisionFilter; }

	ECollisionFilter GetColliderCollisionType() const { return colliderType_; }
	void SetColliderCollisionType(const ECollisionFilter InCollisionFilter) { colliderType_ = InCollisionFilter; }

	bool CanCollideWith(const ColliderComponent& InColliderComponent) const
	{
		return (colliderType_ & InColliderComponent.GetCollisionFilter()) == colliderType_
			&& (InColliderComponent.GetColliderCollisionType() & collisionFilter_) == InColliderComponent.GetColliderCollisionType();
	}

	void OnSendReplication(DataReplicationPacketComponent& OutComponent) override;
	void OnReadReplication(const DataReplicationPacketComponent& InComponent) override;

	bool ShouldUpdateCollisionGrid() const { return bShouldUpdateCollisionGrid_; }
	
	DynamicMulticastDelegate<const ColliderComponent&> triggerEnterDelegate;
	DynamicMulticastDelegate<const ColliderComponent&> triggerExitDelegate;
	DynamicMulticastDelegate<const ColliderComponent&> collisionDelegate;

protected:
	void OnDestruction() override;
	
private:
	bool bShouldUpdateCollisionGrid_ = false;
	Tga::Vector2f positionLastFrame_;

	bool bIsTrigger_ = true;
	
	std::unordered_map<uint16_t, ColliderComponent*> collidersEntered_;
	std::unordered_map<uint16_t, ColliderComponent*> collidersCollidedWithLastUpdate_;
	
	std::shared_ptr<Collider> collider_;

	ECollisionFilter colliderType_ = ECollisionFilter::None;
	ECollisionFilter collisionFilter_ = ECollisionFilter::None;
	
	friend class CollisionGrid;
};
