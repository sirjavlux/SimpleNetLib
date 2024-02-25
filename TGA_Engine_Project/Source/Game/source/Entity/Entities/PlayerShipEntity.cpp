#include "PlayerShipEntity.h"
#include "../EntityComponents/RenderComponent.h"
#include "../EntityComponents/ControllerComponent.h"

void PlayerShipEntity::Init()
{
  RenderComponent* renderComponent = AddComponent<RenderComponent>();
  renderComponent->SetRenderSortingPriority(50);
  
  AddComponent<ControllerComponent>();
}

void PlayerShipEntity::Update(float InDeltaTime)
{
  
}
