#include "ControllerComponent.h"

#include "../../PacketComponents/InputComponent.hpp"
#include "../Entities/Entity.hpp"
#include "Packet/PacketManager.h"
#include "../EntityManager.h"
#include "../RenderManager.h"

void ControllerComponent::Init()
{
  
}

void ControllerComponent::Update(float InDeltaTime)
{
  if (bIsPossessed_)
  {
    const auto& engine = *Tga::Engine::GetInstance();
    if (const HWND focusWind = GetFocus(); focusWind == *engine.GetHWND())
    {
      UpdateInput();
    }
  }
}

void ControllerComponent::SetPossessedBy(const sockaddr_storage& InAddress)
{
  possessedBy_ = InAddress;

  if (Net::PacketManager::Get()->GetManagerType() == ENetworkHandleType::Server)
  {
    EntityManager::Get()->SetPossessedEntityByNetTarget(InAddress, owner_->GetId());
  }
}

void ControllerComponent::UpdatePosition(const float InX, const float InY)
{
  // TODO: Implement better system with position rollback

  owner_->SetPosition({ InX, InY });
  if (EntityManager::Get()->GetPossessedEntity() == owner_)
  {
    RenderManager::Get()->GetCamera().cameraPosition = owner_->GetPosition();
  }
}

void ControllerComponent::UpdateInput()
{
  // Input Direction
  if (GetAsyncKeyState('W'))
  {
    inputDirection_.y += 1.f;
  }
  if (GetAsyncKeyState('S'))
  {
    inputDirection_.y += -1.f;
  }
  if (GetAsyncKeyState('A'))
  {
    inputDirection_.x += -1.f;
  }
  if (GetAsyncKeyState('D'))
  {
    inputDirection_.x += 1.f;
  }

  // Send Input packet
  InputComponent inputComponent;
  inputComponent.entityIdentifier = owner_->GetId();
  inputComponent.xAxis = inputDirection_.x;
  inputComponent.yAxis = inputDirection_.y;
  Net::PacketManager::Get()->SendPacketComponentToParent(inputComponent);

  // Reset Input
  inputDirection_ = { 0.f, 0.f, 0.f };
}
