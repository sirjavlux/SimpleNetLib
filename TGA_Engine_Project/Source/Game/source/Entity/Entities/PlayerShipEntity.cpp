#include "PlayerShipEntity.h"

#include "../EntityManager.h"
#include "../RenderManager.h"
#include "../../Combat/BulletEntity.h"
#include "../../Combat/StatTracker.h"
#include "../Collision/CircleCollider.hpp"
#include "../EntityComponents/RenderComponent.h"
#include "../EntityComponents/ControllerComponent.h"
#include "../EntityComponents/ColliderComponent.h"
#include "../EntityComponents/CombatComponent.h"
#include "../../Locator/Locator.h"

PlayerShipEntity::~PlayerShipEntity()
{

}

void PlayerShipEntity::Init()
{
  
}

void PlayerShipEntity::InitComponents()
{
  RenderComponent* renderComponent = EntityManager::Get()->AddComponentToEntityOfType<RenderComponent>(id_, NetTag("RenderComponent").GetHash());
  renderComponent->SetRenderSortingPriority(50);
  renderComponent->SetSpriteTexture("Sprites/SpaceShip/SpaceShip.png");
  renderComponent->SetSpriteSizeMultiplier({ 4.f, 4.f });

  ControllerComponent* controllerComponent = EntityManager::Get()->AddComponentToEntityOfType<ControllerComponent>(id_, NetTag("ControllerComponent").GetHash());
  
  ColliderComponent* colliderComponent = EntityManager::Get()->AddComponentToEntityOfType<ColliderComponent>(id_, NetTag("ColliderComponent").GetHash());
  std::shared_ptr<CircleCollider> circleCollider = std::make_shared<CircleCollider>();
  circleCollider->radius = 0.03f;
  colliderComponent->SetCollider(circleCollider);
  colliderComponent->SetCollisionFilter(ECollisionFilter::Player | ECollisionFilter::Projectile | ECollisionFilter::WorldDestructible);
  colliderComponent->SetColliderCollisionType(ECollisionFilter::Player);
  
  colliderComponent->triggerEnterDelegate.AddDynamic<PlayerShipEntity>(this, &PlayerShipEntity::OnTriggerEntered);
  colliderComponent->triggerExitDelegate.AddDynamic<PlayerShipEntity>(this, &PlayerShipEntity::OnTriggerExit);

  CombatComponent* combatComponent = EntityManager::Get()->AddComponentToEntityOfType<CombatComponent>(id_, NetTag("CombatComponent").GetHash());
  combatComponent->SetMaxHealth(100.f);
  combatComponent->HealToFullHealth();
  combatComponent->SetCollisionDamage(40.f);

  combatComponent->AddEntityTypeToAffectStats(NetTag("player.ship").GetHash());
  
  combatComponent->entityDeathDelegate.AddDynamic<PlayerShipEntity>(this, &PlayerShipEntity::OnEntityDeath);
}

void PlayerShipEntity::Update(float InDeltaTime)
{
  
}

void PlayerShipEntity::FixedUpdate()
{
  
}

void PlayerShipEntity::OnTriggerEntered(const ColliderComponent& InCollider)
{
  const BulletEntity* bulletEntity = dynamic_cast<const BulletEntity*>(InCollider.GetOwner());
  // Collision with projectile
  if (bulletEntity != nullptr && bulletEntity->GetTypeTagHash() == NetTag("bullet").GetHash()
    && bulletEntity->GetShooterId() != 0 && bulletEntity->GetShooterId() != GetId())
  {
    RenderComponent* renderComponent = GetFirstComponent<RenderComponent>().lock().get();
    renderComponent->SetColor({ 5.f, 0.0f, 0.7f, 1.f });
  }
}

void PlayerShipEntity::OnTriggerExit(const ColliderComponent& InCollider)
{
  
}

void PlayerShipEntity::OnEntityDeath(uint16_t InEnemy)
{
  EntityManager::RespawnPlayerAtRandomPos(this);
}

void PlayerShipEntity::OnReadReplication(const DataReplicationPacketComponent& InComponent)
{
  
}

void PlayerShipEntity::OnSendReplication(DataReplicationPacketComponent& OutComponent)
{
  
}
