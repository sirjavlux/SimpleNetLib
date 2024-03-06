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
			const ColliderComponent* colliderComponent = it->second.lock().get();
			if (colliderComponent != nullptr)
			{
				OnTriggerExit(*colliderComponent);
			}
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

void ColliderComponent::OnColliderCollision(std::weak_ptr<ColliderComponent> InColliderComponent)
{
	const ColliderComponent* colliderComponent = InColliderComponent.lock().get();
	if (colliderComponent == nullptr)
	{
		return;
	}
	
	collisionDelegate.Execute(*colliderComponent);
	
	// Update Triggers Entered and add Collider to cached colliders this frame
	if (collidersCollidedWithLastUpdate_.find(colliderComponent->GetLocalId()) == collidersCollidedWithLastUpdate_.end())
	{
		collidersCollidedWithLastUpdate_.insert({colliderComponent->GetLocalId(), InColliderComponent});
		if (collidersEntered_.find(colliderComponent->GetLocalId()) == collidersEntered_.end())
		{
			collidersEntered_.insert({colliderComponent->GetLocalId(), InColliderComponent});
			OnTriggerEnter(*colliderComponent);
		}
	}
}
