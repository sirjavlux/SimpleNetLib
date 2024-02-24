#pragma once

#include "PacketComponentTypes.hpp"
#include "Packet/PacketComponent.h"

class UpdateEntityPositionComponent : public Net::PacketComponent
{
public:
  UpdateEntityPositionComponent();

  uint16_t entityIdentifier = 0;
  
  float xPos = 0;
  float yPos = 0;
};

inline UpdateEntityPositionComponent::UpdateEntityPositionComponent()
    : PacketComponent(static_cast<int16_t>(EPacketComponentType::UpdateEntityPosition), sizeof(UpdateEntityPositionComponent))
{ }