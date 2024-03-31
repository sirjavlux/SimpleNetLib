#include "stdafx.h"

#include "RenderComponent.h"

#include <codecvt>
#include <locale>

#include "../EntityManager.h"
#include "../RenderManager.h"
#include "..\Entities\Entity.h"

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
  if (Net::PacketManager::Get()->IsServer() || texturePath_.empty() || !bIsVisible_)
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
  spriteInstance_.mySize = Tga::Vector2f{ static_cast<float>(spriteSize_.x), static_cast<float>(spriteSize_.y) } * spriteSizeMultiplier_;
  spriteInstance_.myRotation = std::atan2(owner_->GetDirection().y, owner_->GetDirection().x);
    
  // Cull sprite if outside window bounds
  if ((spriteInstance_.myPosition.x + spriteInstance_.mySize.x / 2.f < 0
    || spriteInstance_.myPosition.y + spriteInstance_.mySize.y / 2.f < 0)
    || (spriteInstance_.myPosition.x - spriteInstance_.mySize.x / 2.f > resolution.x
    || spriteInstance_.myPosition.y - spriteInstance_.mySize.y / 2.f > resolution.y))
  {
    return;
  }
  
  RenderManager::Get()->AddRenderCall(*this);
}

void RenderComponent::SetSpriteTexture(const char* InTexturePath)
{
  texturePath_ = InTexturePath;

  if (!Net::PacketManager::Get()->IsServer())
  {
    const auto& engine = *Tga::Engine::GetInstance();
    const std::wstring wideStr = StringUtility::StringToWideString(InTexturePath);
    sharedData_.myTexture = engine.GetTextureManager().GetTexture(wideStr.c_str());

    spriteSize_ = sharedData_.myTexture->CalculateTextureSize();
  }
  
  textureIdentifier_ = NetTag(InTexturePath);

  // TODO: Temporary debug
  if (!owner_->IsEntityLocal() && !Net::PacketManager::Get()->IsServer())
  {
    static int count = 0;
    std::cout << "Textures set: " << count << "\n";
    ++count;
  }
}

RenderData RenderComponent::GetRenderData() const
{
  return { sortingPriority_, textureIdentifier_.GetHash(), sharedData_ };
}

void RenderComponent::SetColor(const Tga::Color& InColor)
{
  spriteInstance_.myColor = InColor;
}

void RenderComponent::OnSendReplication(DataReplicationPacketComponent& OutComponent)
{
  OutComponent.variableDataObject.SerializeStringMemberVariable(*this, texturePath_);
  
  // Needs to be serialized in same order as deserialized
  OutComponent.variableDataObject.SerializeMemberVariable(*this, spriteSize_);
  OutComponent.variableDataObject.SerializeMemberVariable(*this, spriteSizeMultiplier_);
  OutComponent.variableDataObject.SerializeMemberVariable(*this, sortingPriority_);
  OutComponent.variableDataObject.SerializeMemberVariable(*this, bIsVisible_);
}

void RenderComponent::OnReadReplication(const DataReplicationPacketComponent& InComponent)
{
  // Update texture if new
  const std::string oldTexturePath = texturePath_;
  InComponent.variableDataObject.DeSerializeStringMemberVariable(*this, texturePath_);
  if (texturePath_ != oldTexturePath)
  {
    SetSpriteTexture(texturePath_.c_str());
  }

  // Needs to be serialized in same order as deserialized
  InComponent.variableDataObject.DeSerializeMemberVariable(*this, spriteSize_);
  InComponent.variableDataObject.DeSerializeMemberVariable(*this, spriteSizeMultiplier_);
  InComponent.variableDataObject.DeSerializeMemberVariable(*this, sortingPriority_);
  InComponent.variableDataObject.DeSerializeMemberVariable(*this, bIsVisible_);
}
