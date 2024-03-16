#include "SpriteEntity.h"
#include "../EntityComponents/RenderComponent.h"
#include "../EntityManager.h"

void SpriteEntity::Init()
{
	
}

void SpriteEntity::InitComponents()
{
	RenderComponent* renderComponent = EntityManager::Get()->AddComponentToEntityOfType<RenderComponent>(id_, NetTag("RenderComponent").GetHash());
	renderComponent->SetRenderSortingPriority(0);
}

void SpriteEntity::Update(float InDeltaTime)
{
  
}
