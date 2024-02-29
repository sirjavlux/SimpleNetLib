#include "CollisionManager.h"

#include "CircleCollider.hpp"
#include "Collider.hpp"
#include "../Entities/Entity.hpp"
#include "../EntityComponents/ColliderComponent.h"

void CollisionManager::Initialize()
{
	
}

void CollisionManager::UpdateComponents()
{
	std::vector<int> collidersToRemove;
	int index = 0;
	for (const std::shared_ptr<ColliderComponent> colliderComponentFirst : colliderComponents_)
	{
		++index;
		if (colliderComponentFirst->IsMarkedForRemoval())
		{
			collidersToRemove.push_back(index - 1);
			continue;
		}
		const Collider* firstCollider = colliderComponentFirst->GetCollider();
		if (!firstCollider)
		{
			continue;
		}
		
		for (const std::shared_ptr<ColliderComponent> colliderComponentSecond : colliderComponents_)
		{
			const Collider* secondCollider = colliderComponentSecond->GetCollider();
			if (!secondCollider || colliderComponentSecond->IsMarkedForRemoval())
			{
				continue;
			}

			// Collision Trigger Check
			if (Collision::Intersects(*firstCollider, *secondCollider))
			{
				colliderComponentFirst->OnColliderCollision(*colliderComponentSecond.get());
				colliderComponentSecond->OnColliderCollision(*colliderComponentFirst.get());
			}
		}
	}

	// Remove components
	RemoveComponents(collidersToRemove);
}

void CollisionManager::AddColliderComponent(const std::shared_ptr<ColliderComponent>& InComponent)
{
	colliderComponents_.push_back(InComponent);
}

void CollisionManager::RenderColliderDebugLines()
{
	if (!renderDebugLines_ || Net::PacketManager::Get()->GetManagerType() == ENetworkHandleType::Server)
	{
		return;
	}
	
	for (const std::shared_ptr<ColliderComponent> colliderComponent : colliderComponents_)
	{
		const Collider* collider = colliderComponent->GetCollider();
		switch(collider->type)
		{
		case EColliderType::None:
			break;
		case EColliderType::Circle:
			DrawCircleColliderDebugLines(*reinterpret_cast<const CircleCollider*>(collider), colliderComponent->GetOwner()->GetPosition());
			break;
		}
	}
}

void CollisionManager::RemoveComponents(const std::vector<int>& InComponentIndexes)
{
	for (const int componentIndex : InComponentIndexes)
	{
		colliderComponents_.erase(colliderComponents_.begin() + componentIndex);
	}
}

void CollisionManager::DrawCircleColliderDebugLines(const CircleCollider& InCircleCollider, const Tga::Vector2f& InPosition)
{
	
}
