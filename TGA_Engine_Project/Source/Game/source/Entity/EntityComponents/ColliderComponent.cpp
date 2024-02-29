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

void ColliderComponent::Update(float InDeltaTime)
{
	
}

void ColliderComponent::SetCollider(const std::shared_ptr<Collider>& InCollider)
{
	collider_ = InCollider;
	collider_->owner = owner_;
}

void ColliderComponent::OnColliderCollision(const ColliderComponent& InColliderComponent)
{
	collisionDelegate.Execute(InColliderComponent);
}
