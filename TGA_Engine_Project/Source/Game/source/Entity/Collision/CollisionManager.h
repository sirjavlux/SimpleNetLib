#pragma once

#include <memory>
#include <unordered_map>
#include <vector>

#include "../EntityComponents/ColliderComponent.h"
#include "tge/math/Vector2.h"

struct CircleCollider;
class ColliderComponent;

#define COLLISION_GRID_SIZE 1.2

namespace std
{
	inline void hash_combine(std::size_t& InSeed, const int& InValue)
	{
		InSeed ^= std::hash<int>{}(InValue) + 0x9e3779b9 + (InSeed << 6) + (InSeed >> 2);
	}

	template<> struct hash<Tga::Vector2i>
	{
		std::size_t operator()(const Tga::Vector2i& InPosition) const
		{
			std::size_t hashResult = 0;
			
			hash_combine(hashResult, InPosition.x);
			hash_combine(hashResult, InPosition.y);
			
			return hashResult;
		}
	};
}

struct ColliderComponentMinMaxGridPosition
{
	Tga::Vector2i min;
	Tga::Vector2i max;
};

struct ColliderComponentGridData
{
	std::weak_ptr<ColliderComponent> component;
	ColliderComponentMinMaxGridPosition gridPositions;

	bool operator==(const ColliderComponent* InComponent) const
	{
		return InComponent == component.lock().get();	
	}

	bool operator!=(const ColliderComponent* InComponent) const
	{
		return !(*this == InComponent);	
	}
	
	bool operator==(const std::weak_ptr<ColliderComponent>& InComponent) const
	{
		return InComponent.lock().get() == component.lock().get();	
	}

	bool operator!=(const std::weak_ptr<ColliderComponent>& InComponent) const
	{
		return !(*this == InComponent);	
	}
	
	bool operator==(const ColliderComponentGridData& InData) const
	{
		return InData.component.lock().get() == component.lock().get();	
	}

	bool operator!=(const ColliderComponentGridData& InData) const
	{
		return !(*this == InData);	
	}
};

inline bool operator==(const std::weak_ptr<ColliderComponent>& Lhs, const ColliderComponent* Rhs)
{
	return Lhs.lock().get() == Rhs;
}

using CollisionGridMap = std::unordered_map<Tga::Vector2i, std::vector<std::weak_ptr<ColliderComponent>>>;

// TODO:
class CollisionGrid
{
public:
	void AddColliderComponentToGrid(std::weak_ptr<ColliderComponent> InColliderComponent);
	
	void UpdateCollisionGrid();
	
	void UpdateComponentGridCells(const ColliderComponent* InColliderComponentPtr, ColliderComponentGridData& InComponent);
	void RemoveComponentsByIndexes(const std::unordered_map<Tga::Vector2i, std::vector<int>>& InComponentIndexes);
	
	const std::unordered_map<Tga::Vector2i, std::vector<std::weak_ptr<ColliderComponent>>>& GetColliderGridMap() const { return collisionGridMap_; }

	static void GetGridPositionFromWorldPosition(const Tga::Vector2f& InWorldPosition, Tga::Vector2i& OutGridPosition);
	static void GetColliderComponentMinMaxGridPosition(const ColliderComponent& InColliderComponent, ColliderComponentMinMaxGridPosition& OutMinMax);
	
private:
	void UpdateComponentGridCells(std::weak_ptr<ColliderComponent> InColliderComponent,
		const ColliderComponentMinMaxGridPosition& OldMinMaxData, const ColliderComponentMinMaxGridPosition& NewMinMaxData, bool InIsNewComponent = false);
	
	std::vector<ColliderComponentGridData> colliderComponents_;
	CollisionGridMap collisionGridMap_;
};

// TODO: Needs to check collisions with consideration of net lag
class CollisionManager
{
public:
	void Initialize();

	void UpdateComponents();
	
	void AddColliderComponent(const std::weak_ptr<ColliderComponent>& InComponent);

	void RenderColliderDebugLines();

	void SetShouldRenderDebugLines(const bool InRenderDebugLines) { renderDebugLines_ = InRenderDebugLines; }
	bool GetShouldRenderDebugLines() const { return renderDebugLines_; }
	
private:
	void DrawCircleColliderDebugLines(const CircleCollider& InCircleCollider, const Tga::Vector2f& InPosition);
	
	CollisionGrid collisionGrid_;
	
	bool renderDebugLines_ = false;
};
