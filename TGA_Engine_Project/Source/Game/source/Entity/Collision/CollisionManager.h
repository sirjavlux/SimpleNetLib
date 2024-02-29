#pragma once
#include <memory>
#include <vector>

#include "tge/math/Vector2.h"

struct CircleCollider;
class ColliderComponent;
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
	void RemoveComponents(const std::vector<int>& InComponentIndexes);
	
	void DrawCircleColliderDebugLines(const CircleCollider& InCircleCollider, const Tga::Vector2f& InPosition);
	
	std::vector<std::weak_ptr<ColliderComponent>> colliderComponents_; // TODO: Create grid

	bool renderDebugLines_ = false;
};
