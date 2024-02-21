#include "PlayerShipEntity.h"
#include "../EntityComponents/RenderComponent.h"
#include "../EntityComponents/ControllerComponent.h"

void PlayerShipEntity::Init()
{
  RenderComponent* renderComponent = AddComponent<RenderComponent>();
  // TODO: Set Render Details

  AddComponent<ControllerComponent>();
}

void PlayerShipEntity::Update(float InDeltaTime)
{
  
}
