#pragma once

#include "EntityComponent.hpp"

class RenderComponent : public EntityComponent
{
public:
  void Init() override;
  
  void Update(float InDeltaTime) override;
  
};
