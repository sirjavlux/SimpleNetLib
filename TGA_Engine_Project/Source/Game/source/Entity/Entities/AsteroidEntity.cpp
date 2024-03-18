#include "AsteroidEntity.h"

#include "../EntityManager.h"
#include "../EntityComponents/RenderComponent.h"
#include "../Collision/Collider.hpp"
#include "../Collision/CircleCollider.hpp"
#include "../EntityComponents/ColliderComponent.h"
#include "../../Combat/BulletEntity.h"
#include "../../Combat/StatTracker.h"
#include "../../Locator/Locator.h"
#include "../EntityComponents/CombatComponent.h"
#include "../../Definitions.hpp"

void AsteroidEntity::Init()
{
  
}

void AsteroidEntity::InitComponents()
{
  RenderComponent* renderComponent = EntityManager::Get()->AddComponentToEntityOfType<RenderComponent>(id_, NetTag("RenderComponent").GetHash());
  renderComponent->SetSpriteSizeMultiplier({ 4.f, 4.f });
  renderComponent->SetRenderSortingPriority(40);
  renderComponent->SetSpriteTexture(generationData_.path.c_str());
  
  SetShouldReplicatePosition(true);

  ColliderComponent* colliderComponent = EntityManager::Get()->AddComponentToEntityOfType<ColliderComponent>(id_, NetTag("ColliderComponent").GetHash());
  std::shared_ptr<CircleCollider> circleCollider = std::make_shared<CircleCollider>();
  circleCollider->radius = generationData_.colliderSize;
  colliderComponent->SetCollider(circleCollider);
  colliderComponent->SetCollisionFilter(ECollisionFilter::Player | ECollisionFilter::Projectile);
  colliderComponent->SetColliderCollisionType(ECollisionFilter::WorldDestructible);

  colliderComponent->triggerEnterDelegate.AddDynamic<AsteroidEntity>(this, &AsteroidEntity::OnTriggerEntered);

  CombatComponent* combatComponent = EntityManager::Get()->AddComponentToEntityOfType<CombatComponent>(id_, NetTag("CombatComponent").GetHash());
  combatComponent->SetMaxHealth(50.f);
  combatComponent->HealToFullHealth();
  combatComponent->SetCollisionDamage(38.f);

  combatComponent->entityDeathDelegate.AddDynamic<AsteroidEntity>(this, &AsteroidEntity::OnEntityDeath);
}

void AsteroidEntity::OnSpawnHasBeenReceived()
{
  if (direction_.LengthSqr() < 0.1f)
  {
    // Generate random travel direction
    std::default_random_engine& randEngine = EntityManager::Get()->GetRandomEngine();
    std::uniform_real_distribution distribution(-1.0f, 1.0f);
  
    direction_.x = distribution(randEngine);
    direction_.y = distribution(randEngine);
    direction_.Normalize();

    // Generate Travel Speed
    std::uniform_real_distribution distributionSpeed(0.001f, 0.04f);
    travelSpeed = distributionSpeed(randEngine);
  }
}

void AsteroidEntity::Update(const float InDeltaTime)
{
  
}
void AsteroidEntity::FixedUpdate()
{
  if (Net::PacketManager::Get()->IsServer())
  {
    targetPosition_ += direction_ * travelSpeed * FIXED_UPDATE_DELTA_TIME;

    // Check world bounds, destroy entity if outside world bounds
    if (targetPosition_.x > WORLD_SIZE_X / 1.6f
      || targetPosition_.x < -WORLD_SIZE_X / 1.6f
      || targetPosition_.y > WORLD_SIZE_Y / 1.6f
      || targetPosition_.y < -WORLD_SIZE_Y / 1.6f)
    {
      EntityManager::Get()->MarkEntityForDestruction(GetId());
      Locator::Get()->GetAsteroidManager()->SpawnRandomAsteroidFromMapBorder();
    }
  }
}

void AsteroidEntity::OnTriggerEntered(const ColliderComponent& InCollider)
{
  
}

void AsteroidEntity::OnEntityDeath(uint16_t InEnemy)
{
  // For the moment just remove object from world
  if (EntityManager::Get()->IsServer())
  {
    EntityManager::Get()->DestroyEntityServer(GetId());
  }
}
