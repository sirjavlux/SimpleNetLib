#pragma once

#include "Entity.hpp"

class SpriteEntity : public Entity
{
public:
  void Init() override;
	
  void Update(float InDeltaTime) override;

};
