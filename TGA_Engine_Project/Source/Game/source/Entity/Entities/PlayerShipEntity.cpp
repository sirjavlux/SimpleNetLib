#include "PlayerShipEntity.h"

#include "../../Combat/BulletEntity.h"
#include "../Collision/CircleCollider.hpp"
#include "../EntityComponents/RenderComponent.h"
#include "../EntityComponents/ControllerComponent.h"

void PlayerShipEntity::Init()
{
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

  colliderComponent->triggerEnterDelegate.AddDynamic<PlayerShipEntity>(this, &PlayerShipEntity::OnTriggerEntered);
  colliderComponent->triggerExitDelegate.AddDynamic<PlayerShipEntity>(this, &PlayerShipEntity::OnTriggerExit);
}

void PlayerShipEntity::Update(float InDeltaTime)
{
  
}

void PlayerShipEntity::OnTriggerEntered(const ColliderComponent& InCollider)
{
  const BulletEntity* bulletEntity = static_cast<const BulletEntity*>(InCollider.GetOwner());
  // Collision with projectile
  if (bulletEntity->GetTypeTagHash() == NetTag("bullet").GetHash()
    && bulletEntity->GetShooterId() != 0 && bulletEntity->GetShooterId() != GetId())
  {
    std::cout << "Enemy Hit!\n";
    
    if (Net::PacketManager::Get()->IsServer())
    {
      health_ -= bulletEntity->GetDamage();
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

void PlayerShipEntity::OnReadReplication(DataReplicationPacketComponent& InComponent)
{
  health_ << InComponent;
}

void PlayerShipEntity::OnSendReplication(DataReplicationPacketComponent& OutComponent)
{
  OutComponent << health_;
}