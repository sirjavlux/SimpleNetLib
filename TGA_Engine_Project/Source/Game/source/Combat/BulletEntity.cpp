#include "stdafx.h"

#include "BulletEntity.h"
#include "../Definitions.hpp"
#include "../Entity/EntityManager.h"
#include "../Entity/Collision/CircleCollider.hpp"
#include "../Entity/EntityComponents/ColliderComponent.h"
#include "../Entity/EntityComponents/CombatComponent.h"
#include "../Locator/Locator.h"

void BulletEntity::Init()
{
	
}

void BulletEntity::InitComponents()
{
	RenderComponent* renderComponent = EntityManager::Get()->AddComponentToEntityOfType<RenderComponent>(id_, NetTag("RenderComponent").GetHash());
	renderComponent->SetSpriteTexture("Sprites/SpaceShip/Bullet.png");
	renderComponent->SetSpriteSizeMultiplier({ 3.f, 2.f });
	renderComponent->SetRenderSortingPriority(10);
	
	SetShouldReplicatePosition(true);

	ColliderComponent* colliderComponent = EntityManager::Get()->AddComponentToEntityOfType<ColliderComponent>(id_, NetTag("ColliderComponent").GetHash());
	std::shared_ptr<CircleCollider> circleCollider = std::make_shared<CircleCollider>();
	circleCollider->radius = 0.02f;
	colliderComponent->SetCollider(circleCollider);
	colliderComponent->SetCollisionFilter(ECollisionFilter::Player | ECollisionFilter::WorldDestructible);
	colliderComponent->SetColliderCollisionType(ECollisionFilter::Projectile);

	CombatComponent* combatComponent = EntityManager::Get()->AddComponentToEntityOfType<CombatComponent>(id_, NetTag("CombatComponent").GetHash());
	combatComponent->SetMaxHealth(1.f);
	combatComponent->HealToFullHealth();
	combatComponent->SetCollisionDamage(12.f);

	combatComponent->entityDeathDelegate.AddDynamic<BulletEntity>(this, &BulletEntity::OnEntityDeath);
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
}

void BulletEntity::OnSendReplication(DataReplicationPacketComponent& OutComponent)
{
	OutComponent.variableDataObject.SerializeMemberVariable(*this, shooterId_);
}

void BulletEntity::OnEntityDeath(uint16_t InEnemy)
{
	if (EntityManager::Get()->IsServer())
	{
		EntityManager::Get()->DestroyEntityServer(GetId());
	} else
	{
		GetFirstComponent<RenderComponent>().lock()->SetIsVisible(false);
	}
}
