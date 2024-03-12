#include "PlayerShipEntity.h"

#include "../EntityManager.h"
#include "../../Combat/BulletEntity.h"
#include "../../Combat/StatTracker.h"
#include "../Collision/CircleCollider.hpp"
#include "../EntityComponents/RenderComponent.h"
#include "../EntityComponents/ControllerComponent.h"

void PlayerShipEntity::Init()
{
  if (Net::PacketManager::Get()->IsServer())
  {
    Locator::Get()->GetStatTracker()->SetStat(GetId(), NetTag("health"), maxHealth_);
  }
  
  if (!Net::PacketManager::Get()->IsServer())
  {
    RenderComponent* renderComponent = AddComponent<RenderComponent>().lock().get();
    renderComponent->SetRenderSortingPriority(50);
    renderComponent->SetSpriteTexture("Sprites/SpaceShip/SpaceShip.png");
    renderComponent->SetSpriteSizeMultiplier({ 4.f, 4.f });
  }

  AddComponent<ControllerComponent>();

  ColliderComponent* colliderComponent = AddComponent<ColliderComponent>().lock().get();
  std::shared_ptr<CircleCollider> circleCollider = std::make_shared<CircleCollider>();
  circleCollider->radius = 0.03f;
  colliderComponent->SetCollider(circleCollider);
  colliderComponent->SetCollisionFilter(ECollisionFilter::Player | ECollisionFilter::Projectile);
  colliderComponent->SetColliderCollisionType(ECollisionFilter::Player);
  
  colliderComponent->triggerEnterDelegate.AddDynamic<PlayerShipEntity>(this, &PlayerShipEntity::OnTriggerEntered);
  colliderComponent->triggerExitDelegate.AddDynamic<PlayerShipEntity>(this, &PlayerShipEntity::OnTriggerExit);
}

void PlayerShipEntity::Update(float InDeltaTime)
{
  
}

void PlayerShipEntity::FixedUpdate()
{
  
}

void PlayerShipEntity::OnTriggerEntered(const ColliderComponent& InCollider)
{
  const BulletEntity* bulletEntity = static_cast<const BulletEntity*>(InCollider.GetOwner());
  // Collision with projectile
  if (bulletEntity->GetTypeTagHash() == NetTag("bullet").GetHash()
    && bulletEntity->GetShooterId() != 0 && bulletEntity->GetShooterId() != GetId())
  {
    if (Net::PacketManager::Get()->IsServer())
    {
      TakeDamage(bulletEntity->GetDamage(), bulletEntity->GetShooterId());
    }
    else
    {
      RenderComponent* renderComponent = GetFirstComponent<RenderComponent>().lock().get();
      renderComponent->SetColor({ 5.f, 0.0f, 0.7f, 1.f });
    }
  }
}

void PlayerShipEntity::OnTriggerExit(const ColliderComponent& InCollider)
{
  const BulletEntity* bulletEntity = static_cast<const BulletEntity*>(InCollider.GetOwner());
  // Collision with projectile
  if (bulletEntity->GetTypeTagHash() == NetTag("bullet").GetHash()
    && bulletEntity->GetShooterId() != 0 && bulletEntity->GetShooterId() != GetId())
  {
    if (!Net::PacketManager::Get()->IsServer())
    {
      RenderComponent* renderComponent = GetFirstComponent<RenderComponent>().lock().get();
      renderComponent->SetColor({ 1.f, 1.f, 1.f, 1.f });
    }
  }
}

void PlayerShipEntity::OnReadReplication(const DataReplicationPacketComponent& InComponent)
{
  
}

void PlayerShipEntity::OnSendReplication(DataReplicationPacketComponent& OutComponent)
{
  
}

void PlayerShipEntity::TakeDamage(const int InDamage, const uint16_t InDamagingEntity)
{
  if (Net::PacketManager::Get()->IsServer())
  {
    float currentHealth = Locator::Get()->GetStatTracker()->GetStat(GetId(), NetTag("health")) - InDamage;
    currentHealth = currentHealth < 0 ? 0 : currentHealth;
    Locator::Get()->GetStatTracker()->SetStat(GetId(), NetTag("health"), currentHealth);

    // Death
    if (currentHealth <= 0)
    {
      const float newKillAmount = Locator::Get()->GetStatTracker()->GetStat(InDamagingEntity, NetTag("kills")) + 1;
      Locator::Get()->GetStatTracker()->SetStat(InDamagingEntity, NetTag("kills"), newKillAmount);

      const float newDeathAmount = Locator::Get()->GetStatTracker()->GetStat(GetId(), NetTag("deaths")) + 1;
      Locator::Get()->GetStatTracker()->SetStat(GetId(), NetTag("deaths"), newDeathAmount);

      EntityManager::RespawnPlayerAtRandomPos(this);
    }
  }
}
