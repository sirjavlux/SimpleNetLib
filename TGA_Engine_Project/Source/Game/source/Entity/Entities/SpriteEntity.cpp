#include "SpriteEntity.h"
#include "../EntityComponents/RenderComponent.h"

void SpriteEntity::Init()
{
  std::weak_ptr<RenderComponent> renderComponent = AddComponent<RenderComponent>();
  renderComponent.lock()->SetRenderSortingPriority(0);
}

void SpriteEntity::Update(float InDeltaTime)
{
  
}
