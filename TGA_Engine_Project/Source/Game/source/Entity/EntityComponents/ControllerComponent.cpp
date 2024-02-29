#include "ControllerComponent.h"

#include "../../PacketComponents/UpdateEntityControllerPositionComponent.hpp"
#include "../../PacketComponents/InputComponent.hpp"
#include "../Entities/Entity.hpp"
#include "Packet/PacketManager.h"
#include "../EntityManager.h"
#include "../RenderManager.h"
#include "../../Definitions.hpp"

void ControllerComponent::Init()
{
  
}

void ControllerComponent::Update(float InDeltaTime)
{
  currentDirection_.x = FMath::Lerp(currentDirection_.x, targetDirection_.x, directionLerpSpeed_ * InDeltaTime);
  currentDirection_.y = FMath::Lerp(currentDirection_.y, targetDirection_.y, directionLerpSpeed_ * InDeltaTime);
  
  if (Net::PacketManager::Get()->GetManagerType() == ENetworkHandleType::Client)
  {
    RenderComponent* renderComponent = owner_->GetFirstComponent<RenderComponent>().lock().get();
    if (renderComponent)
    {
      renderComponent->SetDirection(currentDirection_.x, currentDirection_.y);
    }
  }
}

void ControllerComponent::FixedUpdate()
{
  if (Net::PacketManager::Get()->GetManagerType() == ENetworkHandleType::Client)
  {
    UpdateClientPositionFromServerPositionUpdate();
    if (bIsPossessed_)
    {
      UpdateInput();
    }
  }

  UpdateServerPosition();
}

void ControllerComponent::SetPossessedBy(const sockaddr_storage& InAddress)
{
  possessedBy_ = InAddress;

  if (Net::PacketManager::Get()->GetManagerType() == ENetworkHandleType::Server)
  {
    EntityManager::Get()->SetPossessedEntityByNetTarget(InAddress, owner_->GetId());
  }
}

void ControllerComponent::UpdatePositionBuffer(const PositionUpdateEntry& InUpdateEntry)
{
  for (int i = 0; i < POSITION_BUFFER_SIZE - 1; ++i)
  {
    if (InUpdateEntry.sequenceNr > positionUpdatesBuffer_[i].sequenceNr)
    {
      std::memcpy(&positionUpdatesBuffer_[i + 1], &positionUpdatesBuffer_[i], sizeof(PositionUpdateEntry) * (POSITION_BUFFER_SIZE - (1 + i)));
      positionUpdatesBuffer_[i] = InUpdateEntry;
      return;
    }
    if (InUpdateEntry.sequenceNr == positionUpdatesBuffer_[i].sequenceNr)
    {
      positionUpdatesBuffer_[i] = InUpdateEntry;
      return;
    }
  }
}

bool ControllerComponent::UpdateInputBuffer(const InputUpdateEntry& InUpdateEntry)
{
  for (int i = 0; i < INPUT_BUFFER_SIZE - 1; ++i)
  {
    if (InUpdateEntry.sequenceNr > inputHistoryBuffer_[i].sequenceNr)
    {
      std::memcpy(&inputHistoryBuffer_[i + 1], &inputHistoryBuffer_[i], sizeof(InputUpdateEntry) * (INPUT_BUFFER_SIZE - (i + 1)));
      inputHistoryBuffer_[i] = InUpdateEntry;
      return true;
    }
    if (InUpdateEntry.sequenceNr == inputHistoryBuffer_[i].sequenceNr)
    {
      inputHistoryBuffer_[i] = InUpdateEntry;
      return true;
    }
  }
  return false;
}

void ControllerComponent::UpdateServerPosition()
{
  if (Net::PacketManager::Get()->GetManagerType() != ENetworkHandleType::Server)
  {
    return;
  }
  
  Tga::Vector2f newPos = owner_->GetPosition();

  constexpr int indexesBehind = 4;
  for (int i = indexesBehind; i < INPUT_BUFFER_SIZE - 1; ++i)
  {
    const InputUpdateEntry& input = inputHistoryBuffer_[i];
    if (input.sequenceNr <= lastInputSequenceNr_)
    {
      break;
    }
   
    const InputUpdateEntry& inputBehind = inputHistoryBuffer_[i + 1];
    uint32_t sequenceNrBehind = inputBehind.sequenceNr;
    const uint32_t sequenceNr = input.sequenceNr;
    
    // Predict lost inputs
    while (sequenceNr > sequenceNrBehind + 1 && sequenceNr != sequenceNrBehind)
    {
      if (!UpdateInputBuffer({sequenceNrBehind + 1, inputBehind.xInputDir, inputBehind.yInputDir }))
      {
        break; 
      }
      ++sequenceNrBehind;
    }
    
    UpdateVelocity(input.xInputDir, input.yInputDir);
    newPos += velocity_;
  }
  lastInputSequenceNr_ = inputHistoryBuffer_[indexesBehind].sequenceNr;

  // Snap position to map bounds
  newPos.x = newPos.x > WORLD_SIZE_X / 2.f ? WORLD_SIZE_X / 2.f : newPos.x;
  newPos.x = newPos.x < -WORLD_SIZE_X / 2.f ? -WORLD_SIZE_X / 2.f : newPos.x;
  newPos.y = newPos.y > WORLD_SIZE_Y / 2.f ? WORLD_SIZE_Y / 2.f : newPos.y;
  newPos.y = newPos.y < -WORLD_SIZE_Y / 2.f ? -WORLD_SIZE_Y / 2.f : newPos.y;

  PositionUpdateEntry entryData;
  entryData.sequenceNr = lastInputSequenceNr_;
  entryData.xPosition = newPos.x;
  entryData.yPosition = newPos.y;
  entryData.xVelocity = velocity_.x;
  entryData.yVelocity = velocity_.y;
  
  owner_->SetPosition({newPos.x, newPos.y}, true);

  // Send new packet
  UpdateEntityControllerPositionComponent updateEntityPositionComponent;
  updateEntityPositionComponent.entityIdentifier = owner_->GetId();
  updateEntityPositionComponent.positionUpdateEntry = entryData;
  
  updateEntityPositionComponent.overrideDefiningData = updateEntityPositionComponent.entityIdentifier;

  const NetUtility::NetVector3 lodPos = { owner_->GetPosition().x, owner_->GetPosition().y, 0.f };
  Net::PacketManager::Get()->SendPacketComponentMulticastWithLod<UpdateEntityControllerPositionComponent>(updateEntityPositionComponent, lodPos);

  // Update net position for culling
  Net::PacketManager::Get()->UpdateClientNetPosition(possessedBy_, lodPos);

  if (velocity_.LengthSqr() > 0.f)
  {
    targetDirection_ = velocity_.GetNormalized();
  }
}

void ControllerComponent::UpdateClientPositionFromServerPositionUpdate()
{
  constexpr int indexToFetchPositionFrom = 0;
  const PositionUpdateEntry& updateEntry = positionUpdatesBuffer_[indexToFetchPositionFrom];
  Tga::Vector2f newTargetPos = {updateEntry.xPosition, updateEntry.yPosition};
  velocity_ = { updateEntry.xVelocity, updateEntry.yVelocity };
  
  positionUpdateSequenceNr_ = updateEntry.sequenceNr;
  
  // Forward position
  if (bIsPossessed_)
  {
    constexpr int indexesBehind = 8;
    for (int i = indexesBehind; i < INPUT_BUFFER_SIZE; ++i)
    {
      const InputUpdateEntry& entry = inputHistoryBuffer_[i];
      if (entry.sequenceNr <= positionUpdateSequenceNr_)
      {
        break;
      }
      UpdateVelocity(entry.xInputDir, entry.yInputDir);
      newTargetPos += velocity_;
    }
  }

  // Update Target Position
  owner_->SetTargetPosition(newTargetPos);

  // Update target direction
  targetDirection_ = velocity_.GetNormalized();
}

void ControllerComponent::UpdateVelocity(const float InInputX, const float InInputY)
{
  const float xVelocity = InInputX;
  const float yVelocity = InInputY;
  Tga::Vector2f netVector = { xVelocity, yVelocity };
  netVector.Normalize();
  
  netVector *= GetSpeed();
  netVector *= acceleration_;
  velocity_ += netVector;
  velocity_.x -= velocity_.x < 0.f ? resistance_ * -1.f
    : resistance_;
  velocity_.y -= velocity_.y < 0.f ? resistance_ * -1.f
    : resistance_;

  // Clamp minimum velocity
  if (netVector.x == 0 && std::abs(velocity_.x) < 0.0005f)
  {
    velocity_.x = 0.f;
  }
  if (netVector.y == 0 && std::abs(velocity_.y) < 0.0005f)
  {
    velocity_.y = 0.f;
  }

  // Clamp maximum velocity
  if (velocity_.LengthSqr() >= maxVelocity_ * maxVelocity_)
  {
    velocity_.Normalize();
    velocity_ *= maxVelocity_;
  }
}

void ControllerComponent::UpdateInput()
{
  EKeyInput keyInput = EKeyInput::None;
  
  // Input Direction
  const auto& engine = *Tga::Engine::GetInstance();
  if (const HWND focusWind = GetFocus(); focusWind == *engine.GetHWND())
  {
    if (GetAsyncKeyState('W'))
    {
      inputDirection_.y += 1.f;
      keyInput = keyInput | EKeyInput::W;
    }
    if (GetAsyncKeyState('S'))
    {
      inputDirection_.y += -1.f;
      keyInput = keyInput | EKeyInput::S;
    }
    if (GetAsyncKeyState('A'))
    {
      inputDirection_.x += -1.f;
      keyInput = keyInput | EKeyInput::A;
    }
    if (GetAsyncKeyState('D'))
    {
      inputDirection_.x += 1.f;
      keyInput = keyInput | EKeyInput::D;
    }
    if (GetAsyncKeyState(VK_SPACE))
    {
      keyInput = keyInput | EKeyInput::Space;
    }
    if (GetAsyncKeyState(VK_SHIFT))
    {
      keyInput = keyInput | EKeyInput::Shift;
    }
    // Toggle Debug Lines TODO: Needs check if pressed to avoid spamming on and off
    if (GetAsyncKeyState(VK_F5))
    {
      Locator::Get()->GetCollisionManager()->SetShouldRenderDebugLines(!Locator::Get()->GetCollisionManager()->GetShouldRenderDebugLines());
    }
  }

  InputUpdateEntry entry;
  entry.sequenceNr = ++sequenceNumberIter_;
  entry.xInputDir = inputDirection_.x;
  entry.yInputDir = inputDirection_.y;
  
  // Send Input packet
  InputComponent inputComponent;
  inputComponent.entityIdentifier = owner_->GetId();
  inputComponent.sequenceNr = entry.sequenceNr;
  inputComponent.keysPressBuffer = static_cast<uint16_t>(keyInput);
  Net::PacketManager::Get()->SendPacketComponentToParent(inputComponent);

  // Update client input buffer
  UpdateInputBuffer(entry);
  
  // Reset Input
  inputDirection_ = { 0.f, 0.f, 0.f };
}
