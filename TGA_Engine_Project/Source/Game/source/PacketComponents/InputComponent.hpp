#pragma once

#include "PacketComponentTypes.hpp"
#include "../Entity/EntityComponents/ControllerComponent.h"
#include "Packet/PacketComponent.h"

class InputComponent : public Net::PacketComponent
{
public:
  InputComponent();

  uint16_t entityIdentifier = 0;
  
  InputUpdateEntry inputUpdateEntry;
};

inline InputComponent::InputComponent()
    : PacketComponent(static_cast<int16_t>(EPacketComponentType::Input), sizeof(InputComponent))
{ }