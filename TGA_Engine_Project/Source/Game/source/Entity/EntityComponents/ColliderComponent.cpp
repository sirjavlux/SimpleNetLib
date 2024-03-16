#include "ColliderComponent.h"

#include "../../Locator/Locator.h"
#include "../Collision/Collider.hpp"
#include "../Collision/CircleCollider.hpp"
#include "../Collision/CollisionManager.h"
#include "../Entities/Entity.h"

void ColliderComponent::Init()
{
	// TODO: This is ugly, fix this
	for (std::weak_ptr<ColliderComponent> component : owner_->GetComponents<ColliderComponent>())
	{
		if (component.lock().get() && component.lock().get()->id_ == id_)
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
	if (collidersCollidedWithLastUpdate_.find(colliderComponent->GetId()) == collidersCollidedWithLastUpdate_.end())
	{
		collidersCollidedWithLastUpdate_.insert({colliderComponent->GetId(), InColliderComponent});
		if (collidersEntered_.find(colliderComponent->GetId()) == collidersEntered_.end())
		{
			collidersEntered_.insert({colliderComponent->GetId(), InColliderComponent});
			OnTriggerEnter(*colliderComponent);
		}
	}
}

void ColliderComponent::OnSendReplication(DataReplicationPacketComponent& OutComponent)
{
	OutComponent.variableDataObject.SerializeMemberVariable(*this, colliderType_);
	OutComponent.variableDataObject.SerializeMemberVariable(*this, collisionFilter_);
	OutComponent.variableDataObject.SerializeMemberVariable(*this, bIsTrigger_);

	const EColliderType colliderType = collider_ != nullptr ? collider_->type : EColliderType::None;
	OutComponent.variableDataObject.SerializeData(colliderType);
	if (collider_ != nullptr)
	{
		switch (collider_->type)
		{
		case EColliderType::None:
			break;
		case EColliderType::Circle:
		{
			const CircleCollider* circleCollider = dynamic_cast<CircleCollider*>(collider_.get());
			OutComponent.variableDataObject.SerializeData(circleCollider->radius);
			OutComponent.variableDataObject.SerializeData(circleCollider->localPosition);
		}
			break;
		}
	}
}

void ColliderComponent::OnReadReplication(const DataReplicationPacketComponent& InComponent)
{
	InComponent.variableDataObject.DeSerializeMemberVariable(*this, colliderType_);
	InComponent.variableDataObject.DeSerializeMemberVariable(*this, collisionFilter_);
	InComponent.variableDataObject.DeSerializeMemberVariable(*this, bIsTrigger_);

	EColliderType colliderType;
	InComponent.variableDataObject.DeSerializeData(colliderType);
	switch (colliderType)
	{
	case EColliderType::None:
		break;
	case EColliderType::Circle:
	{
		if (collider_ == nullptr || std::static_pointer_cast<CircleCollider>(collider_) == nullptr)
		{
			std::shared_ptr<CircleCollider> circleCollider = std::make_shared<CircleCollider>();
			circleCollider->owner = GetOwner();
			circleCollider->type = EColliderType::Circle;

			InComponent.variableDataObject.DeSerializeData(circleCollider->radius);
			InComponent.variableDataObject.DeSerializeData(circleCollider->localPosition);

			collider_ = circleCollider;
		}
	}
		break;
	}
}
