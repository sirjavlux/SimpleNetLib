#pragma once

#include "EntityComponent.hpp"

class RenderComponent : public EntityComponent
{
public:
  void Init() override;
  
  void Update(float InDeltaTime) override;

  Tga::Sprite2DInstanceData& GetSpriteInstanceData() { return spriteInstance_; }
  Tga::SpriteSharedData& GetSpriteSharedData() { return sharedData_; }

  void SetSpriteTexture(const wchar_t* InTexturePath);
  
private:
  Tga::Sprite2DInstanceData spriteInstance_ = {};
  Tga::SpriteSharedData sharedData_ = {};
};
