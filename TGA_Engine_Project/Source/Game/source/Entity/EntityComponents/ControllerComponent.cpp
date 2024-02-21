#include "ControllerComponent.h"

#include "Packet/PacketManager.h"

void ControllerComponent::Init()
{
  
}

void ControllerComponent::Update(float InDeltaTime)
{
  if (bIsPossessed_)
  {
    UpdateInput();
  }
}

void ControllerComponent::UpdatePosition(const int InX, const int InY)
{
  // TODO: Implement
}

void ControllerComponent::UpdateInput()
{
  // Input Direction
  if (GetAsyncKeyState('A'))
  {
    inputDirection_.x += -1.f;
  }
  if (GetAsyncKeyState('D'))
  {
    inputDirection_.x += 1.f;
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
  // TODO: Send
}
