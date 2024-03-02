#pragma once

#include "EntityComponent.hpp"
#include "Utility/NetVector3.h"
#include "NetIncludes.h"
#include "tge/math/Vector2.h"

// TODO: Implement prediction and movement smoothing during lag

#define POSITION_BUFFER_SIZE 10
#define INPUT_BUFFER_SIZE 40

struct PositionUpdateEntry
{
  uint32_t sequenceNr = 0;
  float xPosition;
  float yPosition;
  float rotation;
  float velocity;
};

struct InputUpdateEntry
{
  uint32_t sequenceNr = 0;
  float xInputDir;
  float yInputDir;
};

namespace Net
{
class PacketComponent;
}
class ControllerComponent : public EntityComponent
{
public:
  void Init() override;
  
  void Update(float InDeltaTime) override;
  void FixedUpdate() override;
  
  void SetPossessed(const bool InShouldBePossessed) { bIsPossessed_ = InShouldBePossessed; }
  void SetPossessedBy(const sockaddr_storage& InAddress);
  bool IsPossessedBy(const sockaddr_storage& InAddress) const { return possessedBy_ == InAddress; }

  float GetSpeed() const { return speed_; }
  void SetSpeed(const float InSpeed) { speed_ = InSpeed; }

  void UpdatePositionBuffer(const PositionUpdateEntry& InUpdateEntry);
  bool UpdateInputBuffer(const InputUpdateEntry& InUpdateEntry);
  
  void UpdateVelocity(float InInputX, float InInputY);

  const Tga::Vector2f& GetVelocity() const { return velocity_; }
  
private:
  void UpdateServerPosition();
  void UpdateClientPositionFromServerPositionUpdate();
  
  void UpdateInput();

  const float directionLerpSpeed_ = 2.f;
  Tga::Vector2f targetDirection_ = { 0, 1 };
  Tga::Vector2f currentDirection_ = { 0, 1 };
  
  float velocity_ = 0.f;

  // Character movement settings
  const float maxVelocity_ = 0.8f;
  const float acceleration_ = 0.02f;
  const float resistanceMultiplier_ = 0.98f;
  const float directionChangeSpeed_ = 2.8f;
  
  NetUtility::NetVector3 inputDirection_ = {};
  
  sockaddr_storage possessedBy_ = {};
  
  float speed_ = 0.8f;
  bool bIsPossessed_ = false;

  uint32_t sequenceNumberIter_ = 1.f;
  
  // High to Low input sequence numbers in buffers
  PositionUpdateEntry positionUpdatesBuffer_[POSITION_BUFFER_SIZE] = {};
  InputUpdateEntry inputHistoryBuffer_[INPUT_BUFFER_SIZE] = {};

  // Client
  uint32_t positionUpdateSequenceNr_ = 0;
  
  // Server
  uint32_t lastInputSequenceNr_ = 0;

  friend class Entity;
};
