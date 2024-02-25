#include "SpriteEntity.h"
#include "../EntityComponents/RenderComponent.h"

void SpriteEntity::Init()
{
  RenderComponent* renderComponent = AddComponent<RenderComponent>();
  renderComponent->SetRenderSortingPriority(0);
}

void SpriteEntity::Update(float InDeltaTime)
{
  
}
