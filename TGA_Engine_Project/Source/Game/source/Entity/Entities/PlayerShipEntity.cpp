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
  if (Net::PacketManager::Get()->IsServer() && InCollider.GetOwner()->GetTypeTagHash() == NetTag("bullet").GetHash())
  {
    const BulletEntity* bulletEntity = static_cast<const BulletEntity*>(InCollider.GetOwner());
    // Collision with projectile
    if (bulletEntity->GetShooterId() != GetId())
    {
      std::cout << "Enemy Hit!\n";
    }
  }
}

void PlayerShipEntity::OnTriggerExit(const ColliderComponent& InCollider)
{
  
}
