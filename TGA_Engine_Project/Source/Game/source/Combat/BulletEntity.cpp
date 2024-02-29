#include "stdafx.h"

#include "BulletEntity.h"
#include "../Definitions.hpp"
#include "../Entity/EntityManager.h"
#include "../Entity/Collision/CircleCollider.hpp"

void BulletEntity::Init()
{
	RenderComponent* renderComponent = AddComponent<RenderComponent>();
	renderComponent->SetRenderSortingPriority(10);
	
	SetShouldReplicatePosition(true);

	ColliderComponent* colliderComponent = AddComponent<ColliderComponent>();
	std::shared_ptr<CircleCollider> circleCollider = std::make_shared<CircleCollider>();
	circleCollider->radius = 0.02f;
	colliderComponent->SetCollider(circleCollider);
}

void BulletEntity::Update(float InDeltaTime)
{

}

void BulletEntity::FixedUpdate()
{
	if (Net::PacketManager::Get()->GetManagerType() == ENetworkHandleType::Client)
	{
		if (RenderComponent* renderComponent = GetComponent<RenderComponent>())
		{
			const Tga::Vector2f direction = GetTargetDirection();
			renderComponent->SetDirection(direction.x, direction.y);
		}
	}
	else if (Net::PacketManager::Get()->GetManagerType() == ENetworkHandleType::Server)
	{
		position_ += travelDirection_ * speed_ * FIXED_UPDATE_DELTA_TIME;

		// Check world bounds, destroy entity if outside world bounds
		if (position_.x > WORLD_SIZE_X / 2.f
			|| position_.x < -WORLD_SIZE_X / 2.f
			|| position_.y > WORLD_SIZE_Y / 2.f
			|| position_.y < -WORLD_SIZE_Y / 2.f)
		{
			EntityManager::Get()->MarkEntityForDestruction(GetId());
		}
	}
}

void BulletEntity::SetTravelDirection(const Tga::Vector2f& InTravelDirection)
{
	travelDirection_ = InTravelDirection;
}
