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

void ControllerComponent::GetCursorPosInScreenSpace(const HWND& InHwd, POINT& OutCursorPoint)
{
  RECT rect;
  RECT clientRect;
  POINT cursorPos;
  if (GetCursorPos(&cursorPos) && GetClientRect(InHwd, &clientRect), GetWindowRect(InHwd, &rect))
  {
    const LONG screenBorderSizeXAxis = ((rect.right - rect.left) - clientRect.right) / 2;
    rect.right -= screenBorderSizeXAxis;
    rect.left += screenBorderSizeXAxis;

    const LONG screenBorderSizeYAxis = ((rect.bottom - rect.top) - clientRect.bottom) / 2;
    rect.bottom -= screenBorderSizeYAxis;
    rect.top += screenBorderSizeYAxis;
      
    cursorPos.x -= rect.left;
    cursorPos.y -= rect.top;
      
    cursorPos.x = std::clamp(cursorPos.x, 0l, rect.right - rect.left);
    cursorPos.y = std::clamp(cursorPos.y, 0l, rect.bottom - rect.top);

    cursorPos.y = clientRect.bottom - cursorPos.y; // Flip y axis
    if (cursorPos.y < 0)
      cursorPos.y = 0;
      
    OutCursorPoint = cursorPos;
  }
}

void ControllerComponent::UpdateServerPosition()
{
  if (!Net::PacketManager::Get()->IsServer())
  {
    return;
  }
  
  Tga::Vector2f newPos = owner_->GetTargetPosition();

  constexpr int indexesBehind = 3;
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
      if (!UpdateInputBuffer({sequenceNrBehind + 1, inputBehind.xInputForce, inputBehind.yInputForce }))
      {
        break; 
      }
      ++sequenceNrBehind;
    }
    
    UpdateVelocity(input.xInputForce, input.yInputForce, input.inputTargetDirection);
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
  entryData.rotation = atan2(targetDirection_.y, targetDirection_.x);
  entryData.xVelocity = velocity_.x;
  entryData.yVelocity = velocity_.y;
  
  owner_->SetTargetPosition({newPos.x, newPos.y});
  
  // Send new packet
  UpdateEntityControllerPositionComponent updateEntityPositionComponent;
  updateEntityPositionComponent.entityIdentifier = owner_->GetId();
  updateEntityPositionComponent.positionUpdateEntry = entryData;
  
  updateEntityPositionComponent.overrideDefiningData = updateEntityPositionComponent.entityIdentifier;

  const NetUtility::NetVector3 lodPos = { owner_->GetTargetPosition().x, owner_->GetTargetPosition().y, 0.f };
  Net::PacketManager::Get()->SendPacketComponentMulticastWithLod<UpdateEntityControllerPositionComponent>(updateEntityPositionComponent, lodPos);
  //Net::PacketManager::Get()->SendPacketComponentMulticast<UpdateEntityControllerPositionComponent>(updateEntityPositionComponent);

  // Update net position for culling
  Net::PacketManager::Get()->UpdateClientNetPosition(possessedBy_, lodPos);
}

void ControllerComponent::UpdateClientPositionFromServerPositionUpdate()
{
  constexpr int indexToFetchPositionFrom = 0;
  const PositionUpdateEntry& updateEntry = positionUpdatesBuffer_[indexToFetchPositionFrom];
  Tga::Vector2f newTargetPos = {updateEntry.xPosition, updateEntry.yPosition};
  targetDirection_ = { std::cos(updateEntry.rotation), std::sin(updateEntry.rotation) };
  velocity_ = { updateEntry.xVelocity, updateEntry.yVelocity };
  
  positionUpdateSequenceNr_ = updateEntry.sequenceNr;
  
  // Forward position
  if (bIsPossessed_)
  {
    constexpr int indexesBehind = 6;
    for (int i = indexesBehind; i < INPUT_BUFFER_SIZE; ++i)
    {
      const InputUpdateEntry& entry = inputHistoryBuffer_[i];
      if (entry.sequenceNr <= positionUpdateSequenceNr_)
      {
        break;
      }
      UpdateVelocity(entry.xInputForce, entry.yInputForce, entry.inputTargetDirection);
      newTargetPos += velocity_;
    }
  }

  // Update Target Position
  owner_->SetTargetPosition(newTargetPos);
}

void ControllerComponent::UpdateVelocity(const float InInputX, const float InInputY, const float InInputTargetDirection)
{
  Tga::Vector2f inputDirection = { InInputX, InInputY };
  inputDirection.Normalize();

  // Calculate new target direction based on input direction
  const Tga::Vector2f inputTargetDirVector = Tga::Vector2f(std::cos(InInputTargetDirection), std::sin(InInputTargetDirection));
  const float rotationOld = std::atan2(targetDirection_.y, targetDirection_.x);

  const float cross = targetDirection_.Cross(inputTargetDirVector);
  const float rotDirNormalized = cross >= 0 ? 1.0f : -1.0f;

  const float newTargetRotation = rotationOld + rotDirNormalized * directionChangeSpeed_ * FIXED_UPDATE_DELTA_TIME;

  const float cosAngle = std::cos(newTargetRotation);
  const float sinAngle = std::sin(newTargetRotation);
  targetDirection_ = { cosAngle, sinAngle };

  // Check if exceeding inputTargetDirVector
  const float crossNew = targetDirection_.Cross(inputTargetDirVector);
  const float rotDirNormalizedNew = crossNew >= 0 ? 1.0f : -1.0f;
  if (rotDirNormalized != rotDirNormalizedNew)
  {
    targetDirection_ = inputTargetDirVector;
  }

  // Calculate velocity
  const Tga::Vector2f forward = targetDirection_;
  const Tga::Vector2f right = targetDirection_.Normal();
  if (inputDirection.LengthSqr() > 0.5)
  {
    const float speedModifier = speed_ * acceleration_ * FIXED_UPDATE_DELTA_TIME;
    velocity_ += forward * inputDirection.y * speedModifier;
    velocity_ += right * inputDirection.x * speedModifier;
  }

  velocity_ *= resistanceMultiplier_;
  
  // Clamp maximum velocity
  if (velocity_.LengthSqr() > maxVelocity_ * maxVelocity_)
  {
    velocity_ = velocity_.GetNormalized() * maxVelocity_;
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
      input_.y += 1.f;
      keyInput = keyInput | EKeyInput::W;
    }
    if (GetAsyncKeyState('S'))
    {
      input_.y += -1.f;
      keyInput = keyInput | EKeyInput::S;
    }
    if (GetAsyncKeyState('A'))
    {
      input_.x += -1.f;
      keyInput = keyInput | EKeyInput::A;
    }
    if (GetAsyncKeyState('D'))
    {
      input_.x += 1.f;
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

    GetCursorPosInScreenSpace(*engine.GetHWND(), lastCursorPos_);
    const Tga::Vector2f shipScreenPos = static_cast<Tga::Vector2f>(engine.GetRenderSize()) / 2.f;
    const Tga::Vector2f cursorPosVector = { static_cast<float>(lastCursorPos_.x), static_cast<float>(lastCursorPos_.y) };
    const Tga::Vector2f direction = (cursorPosVector - shipScreenPos).GetNormalized();
    inputTargetDirection_ = std::atan2(direction.y, direction.x);
    
    //std::cout << "Mouse X: " << lastCursorPos_.x << ", Mouse Y: " << lastCursorPos_.y << std::endl;
  }

  InputUpdateEntry entry;
  entry.sequenceNr = ++sequenceNumberIter_;
  entry.xInputForce = input_.x;
  entry.yInputForce = input_.y;
  entry.inputTargetDirection = inputTargetDirection_;
  
  // Send Input packet
  InputComponent inputComponent;
  inputComponent.entityIdentifier = owner_->GetId();
  inputComponent.sequenceNr = entry.sequenceNr;
  inputComponent.keysPressBuffer = static_cast<uint16_t>(keyInput);
  inputComponent.inputTargetDirection = inputTargetDirection_;
  Net::PacketManager::Get()->SendPacketComponentToParent(inputComponent);

  // Update client input buffer
  UpdateInputBuffer(entry);
  
  // Reset Input
  input_ = { 0.f, 0.f };
}
