#include "stdafx.h"

#include "RenderComponent.h"

#include "../EntityManager.h"
#include "../Entities/Entity.hpp"

#include "Packet/PacketManager.h"

#include "tge/drawers/SpriteDrawer.h"
#include "tge/graphics/GraphicsEngine.h"
#include "tge/texture/TextureManager.h"

void RenderComponent::Init()
{
  if (Net::PacketManager::Get()->GetManagerType() == ENetworkHandleType::Server)
  {
    return;
  }
  
  const auto& engine = *Tga::Engine::GetInstance();

  const Tga::Vector2ui intResolution = engine.GetRenderSize();
  const Tga::Vector2f resolution = { static_cast<float>(intResolution.x), static_cast<float>(intResolution.y) };
  {
    // Default sprite
    sharedData_.myTexture = engine.GetTextureManager().GetTexture(L"Sprites/mygga.png");

    // Pixel-art default settings
    sharedData_.mySamplerAddressMode = Tga::SamplerAddressMode::Clamp;
    sharedData_.mySamplerFilter = Tga::SamplerFilter::Bilinear;
    
    spriteInstance_.myPivot = { 0.5f, 0.5f };
    spriteInstance_.myPosition = owner_->GetPosition() * resolution;
    spriteInstance_.mySize = Tga::Vector2f{ .05f, .05f } * resolution.y;
    spriteInstance_.myColor = Tga::Color(1, 1, 1, 1);
  }
}

void RenderComponent::Update(float InDeltaTime)
{
  if (Net::PacketManager::Get()->GetManagerType() == ENetworkHandleType::Server)
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
  
  spriteInstance_.myPosition = (owner_->GetPosition() - possessedEntityPos) * resolution + resolution / 2.f;
  
  Tga::SpriteDrawer& spriteDrawer(engine.GetGraphicsEngine().GetSpriteDrawer());
  spriteDrawer.Draw(sharedData_, spriteInstance_);
}

void RenderComponent::SetSpriteTexture(const wchar_t* InTexturePath)
{
  const auto& engine = *Tga::Engine::GetInstance();
  sharedData_.myTexture = engine.GetTextureManager().GetTexture(InTexturePath);
}
