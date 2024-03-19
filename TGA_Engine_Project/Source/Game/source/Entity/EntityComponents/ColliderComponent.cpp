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
			Locator::Get()->GetCollisionManager()->AddColliderComponent(component.lock().get());
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
			const ColliderComponent* colliderComponent = it->second;
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

	const Tga::Vector2f newWorldPosition = owner_->GetPosition();
	if (!(positionLastFrame_ == newWorldPosition))
	{
		Tga::Vector2i newGridPos;
		Tga::Vector2i oldGridPos;
		CollisionGrid::GetGridPositionFromWorldPosition(newWorldPosition, newGridPos);
		CollisionGrid::GetGridPositionFromWorldPosition(positionLastFrame_, oldGridPos);
		if (!(newGridPos == oldGridPos))
		{
			bShouldUpdateCollisionGrid_ = true;
		}
	}
	
	positionLastFrame_ = newWorldPosition;
}

void ColliderComponent::SetCollider(const std::shared_ptr<Collider>& InCollider)
{
	collider_ = InCollider;
	collider_->owner = owner_;
	bShouldUpdateCollisionGrid_ = true;
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

void ColliderComponent::OnColliderCollision(ColliderComponent* InColliderComponent)
{
	if (InColliderComponent == nullptr)
	{
		return;
	}
	
	collisionDelegate.Execute(*InColliderComponent);
	
	// Update Triggers Entered and add Collider to cached colliders this frame
	if (collidersCollidedWithLastUpdate_.find(InColliderComponent->GetId()) == collidersCollidedWithLastUpdate_.end())
	{
		collidersCollidedWithLastUpdate_.insert({InColliderComponent->GetId(), InColliderComponent});
		if (collidersEntered_.find(InColliderComponent->GetId()) == collidersEntered_.end())
		{
			collidersEntered_.insert({InColliderComponent->GetId(), InColliderComponent});
			OnTriggerEnter(*InColliderComponent);
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

void ColliderComponent::OnDestruction()
{
	Locator::Get()->GetCollisionManager()->RemoveColliderComponent(this);	
}
