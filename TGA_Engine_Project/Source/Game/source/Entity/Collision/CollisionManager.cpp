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
			if (!colliderComponentSecondPtr || colliderComponentSecondPtr->IsMarkedForRemoval() || colliderComponentSecondPtr->GetLocalId() == colliderComponentFirstPtr->GetLocalId())
			{
				continue;
			}
			const Collider* secondCollider = colliderComponentSecondPtr->GetCollider();
			if (!secondCollider)
			{
				continue;
			}
			
			// Collision Filter Check
			if (!colliderComponentSecondPtr->CanCollideWith(*colliderComponentFirstPtr))
			{
				continue;
			}
			
			// Collision Trigger Check
			if (Collision::Intersects(*firstCollider, *secondCollider))
			{
				colliderComponentFirstPtr->OnColliderCollision(colliderComponentSecond);
				colliderComponentSecondPtr->OnColliderCollision(colliderComponentFirst);
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
	if (!renderDebugLines_ || Net::PacketManager::Get()->IsServer())
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
	for (auto it = colliderComponents_.begin(); it != colliderComponents_.end(); )
	{
		if (std::find(InComponentIndexes.begin(), InComponentIndexes.end(), std::distance(colliderComponents_.begin(), it)) != InComponentIndexes.end())
		{
			it = colliderComponents_.erase(it);
			++removedCount;
		}
		else
		{
			++it;
		}
	}
}

void CollisionManager::DrawCircleColliderDebugLines(const CircleCollider& InCircleCollider, const Tga::Vector2f& InPosition)
{
	const auto& engine = *Tga::Engine::GetInstance();
	Tga::DebugDrawer& debugDrawer = engine.GetDebugDrawer();

	const Tga::Vector2ui intResolution = engine.GetRenderSize();
	const Tga::Vector2f resolution = { static_cast<float>(intResolution.x), static_cast<float>(intResolution.y) };
	const float ratio = engine.GetWindowRatioInversed();
	
	Tga::Vector2f possessedEntityPos;
	if (const Entity* possessedEntity = EntityManager::Get()->GetPossessedEntity(); possessedEntity != nullptr)
	{
		possessedEntityPos = possessedEntity->GetPosition();
	}

	const float hypo = std::sqrt(std::pow(resolution.x, 2.f) + std::pow(resolution.y, 2.f));
	
	debugDrawer.DrawCircle((InPosition - possessedEntityPos) * resolution.y + resolution / 2.f,
		InCircleCollider.radius * hypo * ratio, Tga::Color(1,0,0,1)); // TODO: This doesn't work correctly, radius inaccurate but close enough for some form of representation
}
