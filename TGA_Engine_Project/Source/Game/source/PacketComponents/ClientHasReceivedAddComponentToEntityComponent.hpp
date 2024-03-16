#pragma once

#include "PacketComponentTypes.hpp"
#include "Packet/PacketComponent.h"

class ClientHasReceivedAddComponentToEntityComponent : public Net::PacketComponent
{
public:
	ClientHasReceivedAddComponentToEntityComponent();

	uint16_t entityId;
	uint16_t entityComponentId;
};

inline ClientHasReceivedAddComponentToEntityComponent::ClientHasReceivedAddComponentToEntityComponent()
    : PacketComponent(static_cast<int16_t>(EPacketComponentType::AddEntityComponentHasBeenReceived), sizeof(ClientHasReceivedAddComponentToEntityComponent))
{ }