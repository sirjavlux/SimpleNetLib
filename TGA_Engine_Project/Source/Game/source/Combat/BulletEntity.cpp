#include "stdafx.h"

#include "BulletEntity.h"
#include "../Definitions.hpp"
#include "../Entity/EntityManager.h"
#include "../Entity/Collision/CircleCollider.hpp"

void BulletEntity::Init()
{
	if (!Net::PacketManager::Get()->IsServer())
	{
		RenderComponent* renderComponent = AddComponent<RenderComponent>().lock().get();
		renderComponent->SetSpriteTexture("Sprites/SpaceShip/Bullet.png");
		renderComponent->SetSpriteSizeMultiplier({ 3.f, 2.f });
		renderComponent->SetRenderSortingPriority(10);
	}
	
	SetShouldReplicatePosition(true);

	ColliderComponent* colliderComponent = AddComponent<ColliderComponent>().lock().get();
	std::shared_ptr<CircleCollider> circleCollider = std::make_shared<CircleCollider>();
	circleCollider->radius = 0.02f;
	colliderComponent->SetCollider(circleCollider);
	colliderComponent->SetCollisionFilter(ECollisionFilter::Player);
	colliderComponent->SetColliderCollisionType(ECollisionFilter::Projectile);
}

void BulletEntity::Update(float InDeltaTime)
{

}

void BulletEntity::FixedUpdate()
{
	if (Net::PacketManager::Get()->IsServer())
	{
		targetPosition_ += direction_ * speed_ * FIXED_UPDATE_DELTA_TIME;

		// Check world bounds, destroy entity if outside world bounds
		if (targetPosition_.x > WORLD_SIZE_X / 2.f
			|| targetPosition_.x < -WORLD_SIZE_X / 2.f
			|| targetPosition_.y > WORLD_SIZE_Y / 2.f
			|| targetPosition_.y < -WORLD_SIZE_Y / 2.f)
		{
			EntityManager::Get()->MarkEntityForDestruction(GetId());
		}
	}
}

void BulletEntity::OnReadReplication(const DataReplicationPacketComponent& InComponent)
{
	InComponent.variableDataObject.DeSerializeMemberVariable(*this, shooterId_);
	InComponent.variableDataObject.DeSerializeMemberVariable(*this, damage_);
}

void BulletEntity::OnSendReplication(DataReplicationPacketComponent& OutComponent)
{
	OutComponent.variableDataObject.SerializeMemberVariable(*this, shooterId_);
}
