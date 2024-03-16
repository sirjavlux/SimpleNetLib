#pragma once

#include "Entity.h"

class SpriteEntity : public Entity
{
public:
  void Init() override;
  void InitComponents() override;
  
  void Update(float InDeltaTime) override;

};
