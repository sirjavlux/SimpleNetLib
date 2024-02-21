#pragma once

#include "PacketComponentTypes.hpp"
#include "Packet/PacketComponent.h"

class InputComponent : public Net::PacketComponent
{
public:
  InputComponent();

  uint16_t entityIdentifier = 0;
  
  int8_t xAxis = 0;
  int8_t yAxis = 0;
};

inline InputComponent::InputComponent()
    : PacketComponent(static_cast<int16_t>(EPacketComponentType::Input), sizeof(InputComponent))
{ }