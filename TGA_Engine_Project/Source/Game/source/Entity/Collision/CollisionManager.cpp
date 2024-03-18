#include "stdafx.h"

#include "CollisionManager.h"

#include "CircleCollider.hpp"
#include "Collider.hpp"
#include "../Entities/Entity.h"
#include "../EntityComponents/ColliderComponent.h"
#include "tge/drawers/DebugDrawer.h"
#include "../EntityManager.h"

void CollisionGrid::AddColliderComponentToGrid(std::weak_ptr<ColliderComponent> InColliderComponent)
{
	const ColliderComponent* colliderComponent = InColliderComponent.lock().get();
	if (colliderComponent != nullptr && std::find(colliderComponents_.begin(), colliderComponents_.end(), colliderComponent) == colliderComponents_.end())
	{
		ColliderComponentMinMaxGridPosition outGridMinMaxData;
		GetColliderComponentMinMaxGridPosition(*colliderComponent, outGridMinMaxData);

		const ColliderComponentGridData colliderComponentGridData =
		{
			InColliderComponent,
			outGridMinMaxData
		};

		colliderComponents_.push_back(colliderComponentGridData);

		UpdateComponentGridCells(InColliderComponent, outGridMinMaxData, outGridMinMaxData, true);
	}
}

void CollisionGrid::UpdateCollisionGrid()
{
	std::vector<int> indexesToRemove;
	uint16_t iter = 0;
	for (auto& colliderComponentGridData : colliderComponents_)
	{
		const auto component = colliderComponentGridData.component.lock().get();
		if (!component)
		{
			indexesToRemove.push_back(iter);
			continue;
		}

		// Update collider component cells in grid
		if (component->bShouldUpdateCollisionGrid_)
		{
			UpdateComponentGridCells(component, colliderComponentGridData);
			component->bShouldUpdateCollisionGrid_ = false;
		}

		++iter;
	}

	// Remove invalid collider components
	for (const int& index : indexesToRemove)
	{
		std::swap(colliderComponents_[index], colliderComponents_.back());
		colliderComponents_.pop_back();
	}
}

void CollisionGrid::UpdateComponentGridCells(const ColliderComponent* InColliderComponentPtr, ColliderComponentGridData& InComponent)
{
	ColliderComponentMinMaxGridPosition outGridMinMaxDataNew;
	GetColliderComponentMinMaxGridPosition(*InColliderComponentPtr, outGridMinMaxDataNew);

	UpdateComponentGridCells(InComponent.component, InComponent.gridPositions, outGridMinMaxDataNew);

	InComponent.gridPositions = outGridMinMaxDataNew; // Update to new grid positions
}

void CollisionGrid::RemoveComponentsByIndexes(const std::unordered_map<Tga::Vector2i, std::vector<int>>& InComponentIndexes)
{
	for (const auto& indexes : InComponentIndexes)
	{
		const auto& cell = indexes.first;
		auto& colliderComponents = collisionGridMap_[cell];

		int amountRemoved = 0;
		for (const int index : indexes.second)
		{
			std::swap(colliderComponents[index - amountRemoved], colliderComponents.back());
			colliderComponents.pop_back();

			++amountRemoved;
		}
	}
}

void CollisionGrid::GetGridPositionFromWorldPosition(const Tga::Vector2f& InWorldPosition, Tga::Vector2i& OutGridPosition)
{
	OutGridPosition.x = static_cast<int>(InWorldPosition.x / COLLISION_GRID_SIZE);
	OutGridPosition.y = static_cast<int>(InWorldPosition.y / COLLISION_GRID_SIZE);
}

void CollisionGrid::GetColliderComponentMinMaxGridPosition(const ColliderComponent& InColliderComponent, ColliderComponentMinMaxGridPosition& OutMinMax)
{
	Tga::Vector2f position = InColliderComponent.owner_->GetPosition();

	if (InColliderComponent.collider_ != nullptr)
	{
		position += InColliderComponent.collider_->localPosition;
		switch (InColliderComponent.collider_->type)
		{
		case EColliderType::None:
		{
			GetGridPositionFromWorldPosition(position, OutMinMax.min);
			OutMinMax.max = OutMinMax.min;
		}
			return;
		case EColliderType::Circle:
		{
			const CircleCollider* circleCollider = std::dynamic_pointer_cast<CircleCollider>(InColliderComponent.collider_).get();
			const float rad = circleCollider->radius;
			GetGridPositionFromWorldPosition(position - Tga::Vector2f{ rad, rad }, OutMinMax.min);
			GetGridPositionFromWorldPosition(position + Tga::Vector2f{ rad, rad }, OutMinMax.max);
		}
			return;
		}
	}

	GetGridPositionFromWorldPosition(position, OutMinMax.min);
	OutMinMax.max = OutMinMax.min;
}

void CollisionGrid::UpdateComponentGridCells(std::weak_ptr<ColliderComponent> InColliderComponent,
	const ColliderComponentMinMaxGridPosition& OldMinMaxData, const ColliderComponentMinMaxGridPosition& NewMinMaxData, const bool InIsNewComponent)
{
	const ColliderComponent* InColliderComponentPtr = InColliderComponent.lock().get();
	for (int x = OldMinMaxData.min.x; x <= OldMinMaxData.max.x; ++x)
	{
		for (int y = OldMinMaxData.min.y; y <= OldMinMaxData.max.y; ++y)
		{
			const Tga::Vector2i gridCell = {x, y};
			
			const bool bWasNotIncludedInNewMinMaxData = x < NewMinMaxData.min.x || x > NewMinMaxData.max.x
							|| y < NewMinMaxData.min.y || y > NewMinMaxData.max.y;
			
			// Remove cells no longer in use
			if (!InIsNewComponent && bWasNotIncludedInNewMinMaxData)
			{
				std::vector<std::weak_ptr<ColliderComponent>>& components = collisionGridMap_.at(gridCell);
				auto& iterator = std::find(components.begin(), components.end(), InColliderComponentPtr);
				if (iterator != components.end())
				{
					components.erase(iterator);

					// Erase cell if empty
					if (components.empty())
					{
						collisionGridMap_.erase(gridCell);
					}
				}
			}
				
			// Add to grid cell if new
			if (InIsNewComponent && !bWasNotIncludedInNewMinMaxData)
			{
				collisionGridMap_[{x, y}].push_back(InColliderComponent);
			}
		}
	}

	// Add to non overlapping cells
	for (int x = NewMinMaxData.min.x; x <= NewMinMaxData.max.x; ++x)
	{
		for (int y = NewMinMaxData.min.y; y <= NewMinMaxData.max.y; ++y)
		{
			const bool bWasNotIncludedInOldMinMaxData = x < OldMinMaxData.min.x || x > OldMinMaxData.max.x
				|| y < OldMinMaxData.min.y || y > OldMinMaxData.max.y;
			if (bWasNotIncludedInOldMinMaxData)
			{
				collisionGridMap_[{x, y}].push_back(InColliderComponent);
			}
		}
	}
}

void CollisionManager::Initialize()
{
	
}

void CollisionManager::UpdateComponents()
{
	collisionGrid_.UpdateCollisionGrid();
	
	std::unordered_map<Tga::Vector2i, std::vector<int>> collidersToRemove;

	const std::unordered_map<Tga::Vector2i, std::vector<std::weak_ptr<ColliderComponent>>>& collisionGridMap = collisionGrid_.GetColliderGridMap();
	for (const auto& colliderContainer : collisionGridMap)
	{
		int index = 0;
		for (const std::weak_ptr<ColliderComponent>& colliderComponentFirst : colliderContainer.second)
		{
			ColliderComponent* colliderComponentFirstPtr = colliderComponentFirst.lock().get();
			++index;
			if (!colliderComponentFirstPtr || colliderComponentFirstPtr->IsMarkedForRemoval())
			{
				collidersToRemove[colliderContainer.first].push_back(index - 1); // Needs testing
				continue;
			}
			const Collider* firstCollider = colliderComponentFirstPtr->GetCollider();
			if (!firstCollider)
			{
				continue;
			}
		
			for (const std::weak_ptr<ColliderComponent>& colliderComponentSecond : colliderContainer.second)
			{
				ColliderComponent* colliderComponentSecondPtr = colliderComponentSecond.lock().get();
				if (!colliderComponentSecondPtr || colliderComponentSecondPtr->IsMarkedForRemoval() || colliderComponentSecondPtr->GetId() == colliderComponentFirstPtr->GetId())
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
	}
	
	// Remove components
	collisionGrid_.RemoveComponentsByIndexes(collidersToRemove);
}

void CollisionManager::AddColliderComponent(const std::weak_ptr<ColliderComponent>& InComponent)
{
	collisionGrid_.AddColliderComponentToGrid(InComponent);
}

void CollisionManager::RenderColliderDebugLines()
{
	if (!renderDebugLines_ || Net::PacketManager::Get()->IsServer())
	{
		return;
	}

	const std::unordered_map<Tga::Vector2i, std::vector<std::weak_ptr<ColliderComponent>>>& collisionGridMap = collisionGrid_.GetColliderGridMap();
	for (const auto& colliderContainer : collisionGridMap)
	{
		for (const std::weak_ptr<ColliderComponent>& colliderComponent : colliderContainer.second)
		{
			ColliderComponent* colliderComponentPtr = colliderComponent.lock().get();
			if (!colliderComponentPtr)
			{
				continue;
			}
		
			const Collider* collider = colliderComponentPtr->GetCollider();
			if (!collider)
				return;
		
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