#include "stdafx.h"

#include "RenderComponent.h"

#include <codecvt>
#include <locale>

#include "../EntityManager.h"
#include "../RenderManager.h"
#include "../Entities/Entity.hpp"

#include "Packet/PacketManager.h"

#include "tge/drawers/SpriteDrawer.h"
#include "tge/graphics/GraphicsEngine.h"
#include "tge/texture/TextureManager.h"
#include "Utility/StringUtility.hpp"

void RenderComponent::Init()
{
  if (Net::PacketManager::Get()->IsServer())
  {
    return;
  }
  
  const auto& engine = *Tga::Engine::GetInstance();

  const Tga::Vector2ui intResolution = engine.GetRenderSize();
  const Tga::Vector2f resolution = { static_cast<float>(intResolution.x), static_cast<float>(intResolution.y) };
  {
    // Default sprite
    if (sharedData_.myTexture == nullptr)
    {
      SetSpriteTexture("Sprites/mygga.png");
      spriteSize_ = { 64, 64 };
    }
    
    // Pixel-art default settings
    sharedData_.mySamplerAddressMode = Tga::SamplerAddressMode::Clamp;
    sharedData_.mySamplerFilter = Tga::SamplerFilter::Point;
    
    spriteInstance_.myPivot = { 0.5f, 0.5f };
    spriteInstance_.myPosition = owner_->GetPosition() * resolution;
    spriteInstance_.myColor = Tga::Color(1, 1, 1, 1);
  }
}

void RenderComponent::Update(float InDeltaTime)
{
  if (Net::PacketManager::Get()->IsServer())
  {
    return;
  }
  
  const auto& engine = *Tga::Engine::GetInstance();
  
  const Tga::Vector2ui intResolution = engine.GetRenderSize();
  const Tga::Vector2f resolution = { static_cast<float>(intResolution.x), static_cast<float>(intResolution.y) };

  // Offset for using possessed entity as camera focus
  Tga::Vector2f possessedEntityPos;
  if (const Entity* possessedEntity = EntityManager::Get()->GetPossessedEntity(); possessedEntity != nullptr)
  {
    possessedEntityPos = possessedEntity->GetPosition();
  }
  
  spriteInstance_.myPosition = (owner_->GetPosition() - possessedEntityPos) * resolution.y + resolution / 2.f;
  spriteInstance_.mySize = spriteSize_ * spriteSizeMultiplier_;
  spriteInstance_.myRotation = std::atan2(owner_->GetDirection().y, owner_->GetDirection().x);
    
  // Cull sprite if outside window bounds
  if (((spriteInstance_.myPosition.x < 0 && spriteInstance_.myPosition.x < spriteInstance_.mySize.x)
    || (spriteInstance_.myPosition.y < 0 && spriteInstance_.myPosition.y < spriteInstance_.mySize.y))
    || ((spriteInstance_.myPosition.x > resolution.x && spriteInstance_.myPosition.x - resolution.x > spriteInstance_.mySize.x)
    || (spriteInstance_.myPosition.y > resolution.y && spriteInstance_.myPosition.y - resolution.y > spriteInstance_.mySize.y)))
  {
    return;
  }
  
  RenderManager::Get()->AddRenderCall(*this);
}

void RenderComponent::SetSpriteTexture(const char* InTexturePath)
{
  const auto& engine = *Tga::Engine::GetInstance();
  const std::wstring wideStr = StringUtility::StringToWideString(InTexturePath);
  sharedData_.myTexture = engine.GetTextureManager().GetTexture(wideStr.c_str());

  spriteSize_ = sharedData_.myTexture->CalculateTextureSize();
  
  textureIdentifier_ = NetTag(InTexturePath);
}

RenderData RenderComponent::GetRenderData() const
{
  return { sortingPriority_, textureIdentifier_.GetHash(), sharedData_ };
}

void RenderComponent::SetColor(const Tga::Color& InColor)
{
  spriteInstance_.myColor = InColor;
}
