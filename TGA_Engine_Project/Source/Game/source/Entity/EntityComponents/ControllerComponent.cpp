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
  owner_->SetDirection({currentDirection_.x, currentDirection_.y});

  // std::cout << "Rotation " << std::atan2(currentDirection_.y, currentDirection_.x) << " Target Rotation " << std::atan2(targetDirection_.y, targetDirection_.x) << "\n";
}

void ControllerComponent::FixedUpdate()
{
  if (!Net::PacketManager::Get()->IsServer())
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

  if (Net::PacketManager::Get()->IsServer())
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
  if (!Net::PacketManager::Get()->IsServer())
  {
    return;
  }
  
  Tga::Vector2f newPos = owner_->GetPosition();

  constexpr int indexesBehind = 2;
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
    newPos += targetDirection_ * velocity_;
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
  entryData.rotation = atan2(targetDirection_.y, targetDirection_.x);
  entryData.velocity = velocity_;
  
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
}

void ControllerComponent::UpdateClientPositionFromServerPositionUpdate()
{
  constexpr int indexToFetchPositionFrom = 0;
  const PositionUpdateEntry& updateEntry = positionUpdatesBuffer_[indexToFetchPositionFrom];
  Tga::Vector2f newTargetPos = {updateEntry.xPosition, updateEntry.yPosition};
  targetDirection_ = { std::cos(updateEntry.rotation), std::sin(updateEntry.rotation) };
  velocity_ = updateEntry.velocity;
  
  positionUpdateSequenceNr_ = updateEntry.sequenceNr;
  
  // Forward position
  if (bIsPossessed_)
  {
    constexpr int indexesBehind = 4;
    for (int i = indexesBehind; i < INPUT_BUFFER_SIZE; ++i)
    {
      const InputUpdateEntry& entry = inputHistoryBuffer_[i];
      if (entry.sequenceNr <= positionUpdateSequenceNr_)
      {
        break;
      }
      UpdateVelocity(entry.xInputDir, entry.yInputDir);
      newTargetPos += targetDirection_ * velocity_;
    }
  }

  // Update Target Position
  owner_->SetTargetPosition(newTargetPos);
}

void ControllerComponent::UpdateVelocity(const float InInputX, const float InInputY)
{
  Tga::Vector2f inputDirection = { InInputX, InInputY };
  inputDirection.Normalize();
  
  targetDirection_ += inputDirection * directionChangeSpeed_ * FIXED_UPDATE_DELTA_TIME;
  targetDirection_.Normalize();

  float dot = inputDirection.Dot(targetDirection_);
  if (dot < 0.3f)
  {
    dot = 0.f;
  }
  
  if (inputDirection.LengthSqr() > 0.5)
  {
    velocity_ += dot * speed_ * acceleration_ * FIXED_UPDATE_DELTA_TIME;
  }

  velocity_ *= resistanceMultiplier_;
  
  // Clamp maximum velocity
  if (velocity_ > maxVelocity_ * FIXED_UPDATE_DELTA_TIME)
  {
    velocity_ = maxVelocity_ * FIXED_UPDATE_DELTA_TIME;
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
    // Toggle Debug Lines
    static bool bIsDebugKeyPressed = false;
    if (GetAsyncKeyState(VK_F5) & 0x8000 && !bIsDebugKeyPressed)
    {
      Locator::Get()->GetCollisionManager()->SetShouldRenderDebugLines(!Locator::Get()->GetCollisionManager()->GetShouldRenderDebugLines());
      bIsDebugKeyPressed = true;
    }
    if (!GetAsyncKeyState(VK_F5) && bIsDebugKeyPressed)
    {
      bIsDebugKeyPressed = false;
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
