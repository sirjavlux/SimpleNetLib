#pragma once

#include "EntityComponent.hpp"
#include "Utility/NetVector3.h"
#include "NetIncludes.h"

// TODO: Implement prediction and movement smoothing during lag

#define POSITION_BUFFER_SIZE 20
#define INPUT_BUFFER_SIZE 30

struct PositionUpdateEntry
{
  uint32_t sequenceNr = 0;
  float xPosition;
  float yPosition;
  float xVelocity;
  float yVelocity;
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
  void FixedUpdate(float InDeltaTime) override;
  
  void SetPossessed(const bool InShouldBePossessed) { bIsPossessed_ = InShouldBePossessed; }
  void SetPossessedBy(const sockaddr_storage& InAddress);
  bool IsPossessedBy(const sockaddr_storage& InAddress) const { return possessedBy_ == InAddress; }

  float GetSpeed() const { return speed_; }
  void SetSpeed(const float InSpeed) { speed_ = InSpeed; }

  void UpdatePositionBuffer(const PositionUpdateEntry& InUpdateEntry); // TODO: Implement better version
  void UpdateInputBuffer(const InputUpdateEntry& InUpdateEntry);

  PositionUpdateEntry FetchNewServerPosition();
  
  void UpdateVelocity(float InInputX, float InInputY);

  const NetUtility::NetVector3& GetVelocity() const { return velocity_; }
  
private:
  void UpdateInput();

  const float directionLerpSpeed_ = 2.f;
  NetUtility::NetVector3 targetDirection_ = {};
  NetUtility::NetVector3 currentDirection_ = {};
  
  NetUtility::NetVector3 velocity_ = {};

  // Character movement settings
  const float maxVelocity_ = 0.5f * FIXED_UPDATE_TIME;
  const float acceleration_ = 0.04f * FIXED_UPDATE_TIME;
  const float resistance_ = 0.004f * FIXED_UPDATE_TIME;
  
  NetUtility::NetVector3 inputDirection_ = {};
  
  sockaddr_storage possessedBy_ = {};
  
  float speed_ = 0.5f;
  bool bIsPossessed_ = false;

  uint32_t sequenceNumberIter_ = 1.f;
  
  // High to Low input sequence numbers in buffers
  PositionUpdateEntry positionUpdatesBuffer_[POSITION_BUFFER_SIZE] = {};
  InputUpdateEntry inputHistoryBuffer_[INPUT_BUFFER_SIZE] = {};

  // Server
  uint32_t lastInputSequenceNr_ = 0;
};
