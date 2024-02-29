#include "PlayerShipEntity.h"

#include "../Collision/CircleCollider.hpp"
#include "../EntityComponents/RenderComponent.h"
#include "../EntityComponents/ControllerComponent.h"

void PlayerShipEntity::Init()
{
  RenderComponent* renderComponent = AddComponent<RenderComponent>();
  renderComponent->SetRenderSortingPriority(50);
  
  AddComponent<ControllerComponent>();

  ColliderComponent* colliderComponent = AddComponent<ColliderComponent>();
  std::shared_ptr<CircleCollider> circleCollider = std::make_shared<CircleCollider>();
  circleCollider->radius = 0.1f;
  colliderComponent->SetCollider(circleCollider);

  colliderComponent->collisionDelegate.AddDynamic<PlayerShipEntity>(this, &PlayerShipEntity::OnCollision);
}

void PlayerShipEntity::Update(float InDeltaTime)
{
  
}

void PlayerShipEntity::OnCollision(const ColliderComponent& InCollider)
{
  
}
