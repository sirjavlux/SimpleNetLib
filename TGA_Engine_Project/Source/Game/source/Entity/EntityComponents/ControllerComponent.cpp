#include "ControllerComponent.h"

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
  if (Net::PacketManager::Get()->GetManagerType() == ENetworkHandleType::Client)
  {
    currentDirection_.x = FMath::Lerp(currentDirection_.x, targetDirection_.x, directionLerpSpeed_ * InDeltaTime);
    currentDirection_.y = FMath::Lerp(currentDirection_.y, targetDirection_.y, directionLerpSpeed_ * InDeltaTime);

    RenderComponent* renderComponent = owner_->GetComponent<RenderComponent>();
    if (renderComponent)
    {
      renderComponent->SetDirection(currentDirection_.x, currentDirection_.y);
    }
  }
}

void ControllerComponent::FixedUpdate(float InDeltaTime)
{
  if (Net::PacketManager::Get()->GetManagerType() == ENetworkHandleType::Client && bIsPossessed_)
  {
    UpdateInput();
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

void ControllerComponent::UpdateInputBuffer(const InputUpdateEntry& InUpdateEntry)
{
  for (int i = 0; i < INPUT_BUFFER_SIZE - 1; ++i)
  {
    if (InUpdateEntry.sequenceNr > inputHistoryBuffer_[i].sequenceNr)
    {
      std::memcpy(&inputHistoryBuffer_[i + 1], &inputHistoryBuffer_[i], sizeof(InputUpdateEntry) * (INPUT_BUFFER_SIZE - (i + 1)));
      inputHistoryBuffer_[i] = InUpdateEntry;
      return;
    }
    if (InUpdateEntry.sequenceNr == inputHistoryBuffer_[i].sequenceNr)
    {
      inputHistoryBuffer_[i] = InUpdateEntry;
      return;
    }
  }
}

PositionUpdateEntry ControllerComponent::FetchNewServerPosition()
{
  PositionUpdateEntry result = {};
  if (Net::PacketManager::Get()->GetManagerType() != ENetworkHandleType::Server)
  {
    return result;
  }
  
  Tga::Vector2f newPos = owner_->GetPosition();
  
  for (int i = 0; i < INPUT_BUFFER_SIZE; ++i)
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
      UpdateInputBuffer({sequenceNrBehind + 1, inputBehind.xInputDir, inputBehind.yInputDir });
      ++sequenceNrBehind;
    }
    
    UpdateVelocity(input.xInputDir, input.yInputDir);
    newPos += { velocity_.x, velocity_.y };
  }
  lastInputSequenceNr_ = inputHistoryBuffer_[0].sequenceNr;

  // Snap position to map bounds
  newPos.x = newPos.x > WORLD_SIZE_X / 2.f ? WORLD_SIZE_X / 2.f : newPos.x;
  newPos.x = newPos.x < -WORLD_SIZE_X / 2.f ? -WORLD_SIZE_X / 2.f : newPos.x;
  newPos.y = newPos.y > WORLD_SIZE_Y / 2.f ? WORLD_SIZE_Y / 2.f : newPos.y;
  newPos.y = newPos.y < -WORLD_SIZE_Y / 2.f ? -WORLD_SIZE_Y / 2.f : newPos.y;
  
  result.sequenceNr = lastInputSequenceNr_;
  result.xPosition = newPos.x;
  result.yPosition = newPos.y;
  result.xVelocity = velocity_.x;
  result.yVelocity = velocity_.y;

  owner_->SetPosition({newPos.x, newPos.y});
  
  return result;
}

void ControllerComponent::UpdateVelocity(const float InInputX, const float InInputY)
{
  // TODO: Needs to be compatible with rollback and forward system
  
  const float xVelocity = InInputX;
  const float yVelocity = InInputY;
  NetUtility::NetVector3 netVector = { xVelocity, yVelocity, 0.f };
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

  // Update target direction
  if (Net::PacketManager::Get()->GetManagerType() == ENetworkHandleType::Client)
  {
    targetDirection_ = velocity_.GetNormalized();
  }
}

void ControllerComponent::UpdateInput()
{
  // Input Direction
  const auto& engine = *Tga::Engine::GetInstance();
  if (const HWND focusWind = GetFocus(); focusWind == *engine.GetHWND())
  {
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
  }
  
  // Send Input packet
  InputComponent inputComponent;
  inputComponent.entityIdentifier = owner_->GetId();
  inputComponent.inputUpdateEntry.sequenceNr = ++sequenceNumberIter_;
  inputComponent.inputUpdateEntry.xInputDir = inputDirection_.x;
  inputComponent.inputUpdateEntry.yInputDir = inputDirection_.y;
  Net::PacketManager::Get()->SendPacketComponentToParent(inputComponent);
  
  // Reset Input
  inputDirection_ = { 0.f, 0.f, 0.f };
}
