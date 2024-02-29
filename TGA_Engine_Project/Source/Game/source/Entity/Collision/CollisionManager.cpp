#include "CollisionManager.h"

#include "CircleCollider.hpp"
#include "Collider.hpp"
#include "../Entities/Entity.hpp"
#include "../EntityComponents/ColliderComponent.h"
#include "tge/drawers/CustomShapeDrawer.h"
#include "tge/drawers/DebugDrawer.h"
#include "tge/graphics/GraphicsEngine.h"
#include "../EntityManager.h"

void CollisionManager::Initialize()
{
	
}

void CollisionManager::UpdateComponents()
{
	std::vector<int> collidersToRemove;
	int index = 0;
	for (const std::weak_ptr<ColliderComponent> colliderComponentFirst : colliderComponents_)
	{
		ColliderComponent* colliderComponentFirstPtr = colliderComponentFirst.lock().get();
		++index;
		if (!colliderComponentFirstPtr || colliderComponentFirstPtr->IsMarkedForRemoval())
		{
			collidersToRemove.push_back(index - 1);
			continue;
		}
		const Collider* firstCollider = colliderComponentFirstPtr->GetCollider();
		if (!firstCollider)
		{
			continue;
		}
		
		for (const std::weak_ptr<ColliderComponent> colliderComponentSecond : colliderComponents_)
		{
			ColliderComponent* colliderComponentSecondPtr = colliderComponentSecond.lock().get();
			if (!colliderComponentSecondPtr || colliderComponentSecondPtr->IsMarkedForRemoval() || colliderComponentSecondPtr == colliderComponentFirstPtr)
			{
				continue;
			}
			const Collider* secondCollider = colliderComponentSecondPtr->GetCollider();
			if (!secondCollider)
			{
				continue;
			}

			// Collision Trigger Check
			if (Collision::Intersects(*firstCollider, *secondCollider))
			{
				colliderComponentFirstPtr->OnColliderCollision(*colliderComponentSecondPtr);
				colliderComponentSecondPtr->OnColliderCollision(*colliderComponentFirstPtr);
			}
		}
	}

	// Remove components
	RemoveComponents(collidersToRemove);
}

void CollisionManager::AddColliderComponent(const std::weak_ptr<ColliderComponent>& InComponent)
{
	colliderComponents_.push_back(InComponent);
}

void CollisionManager::RenderColliderDebugLines()
{
	if (!renderDebugLines_ || Net::PacketManager::Get()->GetManagerType() == ENetworkHandleType::Server)
	{
		return;
	}
	
	for (const std::weak_ptr<ColliderComponent> colliderComponent : colliderComponents_)
	{
		ColliderComponent* colliderComponentPtr = colliderComponent.lock().get();
		if (!colliderComponentPtr)
		{
			continue;
		}
		
		const Collider* collider = colliderComponentPtr->GetCollider();
		switch(collider->type)
		{
		case EColliderType::None:
			break;
		case EColliderType::Circle:
			DrawCircleColliderDebugLines(*reinterpret_cast<const CircleCollider*>(collider), colliderComponentPtr->GetOwner()->GetPosition());
			break;
		}
	}
}

void CollisionManager::RemoveComponents(const std::vector<int>& InComponentIndexes)
{
	int removedCount = 0;
	for (const int componentIndex : InComponentIndexes)
	{
		colliderComponents_.erase(colliderComponents_.begin() + componentIndex - removedCount);
		++removedCount;
	}
}

void CollisionManager::DrawCircleColliderDebugLines(const CircleCollider& InCircleCollider, const Tga::Vector2f& InPosition)
{
	const auto& engine = *Tga::Engine::GetInstance();
	Tga::DebugDrawer& debugDrawer = engine.GetDebugDrawer();

	const Tga::Vector2ui intResolution = engine.GetRenderSize();
	const Tga::Vector2f resolution = { static_cast<float>(intResolution.x), static_cast<float>(intResolution.y) };

	Tga::Vector2f possessedEntityPos;
	if (const Entity* possessedEntity = EntityManager::Get()->GetPossessedEntity(); possessedEntity != nullptr)
	{
		possessedEntityPos = possessedEntity->GetPosition();
	}
	
	debugDrawer.DrawCircle((InPosition - possessedEntityPos) * resolution + resolution / 2.f,
		InCircleCollider.radius * resolution.y, Tga::Color(1,0,0,1)); // TODO: This doesn't work correctly, radius inaccurate
}
