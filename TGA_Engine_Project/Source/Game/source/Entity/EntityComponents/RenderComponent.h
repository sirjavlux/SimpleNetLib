#pragma once
#include "EntityComponent.hpp"

class RenderComponent : public EntityComponent
{
public:
  void Update(float InDeltaTime) override;
};
