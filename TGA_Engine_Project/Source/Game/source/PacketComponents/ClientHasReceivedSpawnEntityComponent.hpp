#pragma once

#include "PacketComponentTypes.hpp"
#include "Packet/PacketComponent.h"

class ClientHasReceivedSpawnEntityComponent : public Net::PacketComponent
{
public:
	ClientHasReceivedSpawnEntityComponent();

	uint16_t entityId;
};

inline ClientHasReceivedSpawnEntityComponent::ClientHasReceivedSpawnEntityComponent()
    : PacketComponent(static_cast<int16_t>(EPacketComponentType::SpawnEntityHasBeenReceived), sizeof(ClientHasReceivedSpawnEntityComponent))
{ }