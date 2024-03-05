#include "ColliderComponent.h"

#include "../../Locator/Locator.h"
#include "../Collision/Collider.hpp"
#include "../Collision/CollisionManager.h"
#include "../Entities/Entity.hpp"

void ColliderComponent::Init()
{
	// TODO: This is ugly, fix this
	for (std::weak_ptr<ColliderComponent> component : owner_->GetComponents<ColliderComponent>())
	{
		if (component.lock().get() && component.lock().get()->localId_ == localId_)
		{
			Locator::Get()->GetCollisionManager()->AddColliderComponent(component);
		}
	}
}

void ColliderComponent::FixedUpdate()
{
	std::set<uint16_t> collidersToRemove;

	// Check colliders exited
	for (auto it = collidersEntered_.begin(); it != collidersEntered_.end(); ++it)
	{
		const uint16_t& colliderIndex = it->first;
		if (collidersCollidedWithLastUpdate_.find(colliderIndex) == collidersCollidedWithLastUpdate_.end())
		{
			OnTriggerExit(it->second);
			collidersToRemove.insert(colliderIndex);
		}
	}

	// Remove colliders
	for (const auto& colliderIndex : collidersToRemove)
	{
		collidersEntered_.erase(colliderIndex);
	}
	
	collidersCollidedWithLastUpdate_.clear();
	positionLastFrame_ = owner_->GetPosition();
}

void ColliderComponent::SetCollider(const std::shared_ptr<Collider>& InCollider)
{
	collider_ = InCollider;
	collider_->owner = owner_;
}

void ColliderComponent::OnTriggerEnter(const ColliderComponent& InColliderComponent)
{
	if (bIsTrigger_)
		triggerEnterDelegate.Execute(InColliderComponent);
}

void ColliderComponent::OnTriggerExit(const ColliderComponent& InColliderComponent)
{
	if (bIsTrigger_)
		triggerExitDelegate.Execute(InColliderComponent);
}

void ColliderComponent::OnColliderCollision(const ColliderComponent& InColliderComponent)
{
	collisionDelegate.Execute(InColliderComponent);
	
	// Update Triggers Entered and add Collider to cached colliders this frame
	if (collidersCollidedWithLastUpdate_.find(InColliderComponent.GetLocalId()) == collidersCollidedWithLastUpdate_.end())
	{
		collidersCollidedWithLastUpdate_.insert({InColliderComponent.GetLocalId(), InColliderComponent});
		if (collidersEntered_.find(InColliderComponent.GetLocalId()) == collidersEntered_.end())
		{
			collidersEntered_.insert({InColliderComponent.GetLocalId(), InColliderComponent});
			OnTriggerEnter(InColliderComponent);
		}
	}
}
