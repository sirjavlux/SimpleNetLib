#pragma once

#include "PacketComponentTypes.hpp"
#include "Packet/PacketComponent.h"

class RequestDeSpawnEntityComponent : public Net::PacketComponent
{
public:
  RequestDeSpawnEntityComponent();
};

inline RequestDeSpawnEntityComponent::RequestDeSpawnEntityComponent()
    : PacketComponent(static_cast<int16_t>(EPacketComponentType::RequestDeSpawnEntity), sizeof(RequestDeSpawnEntityComponent))
{ }