#pragma once

#include "EntityComponent.hpp"
#include "Utility/NetVector3.h"
#include "NetIncludes.h"

// TODO: Implement prediction and movement smoothing during lag

namespace Net
{
class PacketComponent;
}
class ControllerComponent : public EntityComponent
{
public:
  void Init() override;
  
  void Update(float InDeltaTime) override;
  
  void SetPossessed(const bool InShouldBePossessed) { bIsPossessed_ = InShouldBePossessed; }
  void SetPossessedBy(const sockaddr_storage& InAddress);
  bool IsPossessedBy(const sockaddr_storage& InAddress) const { return possessedBy_ == InAddress; }

  float GetSpeed() const { return speed_; }
  void SetSpeed(const float InSpeed) { speed_ = InSpeed; }

  void UpdatePosition(float InX, float InY); // TODO: Implement better version

  void UpdateVelocity(float InInputX, float InInputY);

  const NetUtility::NetVector3& GetVelocity() const { return velocity_; }
  
private:
  void UpdateInput();

  NetUtility::NetVector3 velocity_;

  // Character movement settings
  const float maxVelocity_ = 0.5f * FIXED_UPDATE_TIME;
  const float acceleration_ = 0.04f * FIXED_UPDATE_TIME;
  const float resistance_ = 0.004f * FIXED_UPDATE_TIME;
  
  NetUtility::NetVector3 inputDirection_;
  
  sockaddr_storage possessedBy_;
  
  float speed_ = 0.5f;
  bool bIsPossessed_ = false;
};
