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
  void SetPossessedBy(const sockaddr_storage& InAddress) { possessedBy_ = InAddress; }
  bool IsPossessedBy(const sockaddr_storage& InAddress) const { return possessedBy_ == InAddress; }
  
  void SetSpeed(const float InSpeed) { speed_ = InSpeed; }

  void UpdatePosition(int InX, int InY); // TODO: Implement
  
private:
  void UpdateInput();
  
  NetUtility::NetVector3 inputDirection_;

  sockaddr_storage possessedBy_;
  
  float speed_ = 1.f;
  bool bIsPossessed_ = false;
};
