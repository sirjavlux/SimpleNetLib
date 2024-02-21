#pragma once

#include "Entity.hpp"

class PlayerShipEntity : public Entity
{
public:
  void Init() override;
	
  void Update(float InDeltaTime) override;

};
