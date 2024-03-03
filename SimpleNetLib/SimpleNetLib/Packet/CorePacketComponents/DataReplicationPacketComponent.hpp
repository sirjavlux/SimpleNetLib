#pragma once

#include "../PacketComponent.h"

class NET_LIB_EXPORT DataReplicationPacketComponent : public Net::PacketComponent
{
public:
	explicit DataReplicationPacketComponent();

	uint8_t data[NET_PACKET_COMPONENT_DATA_SIZE_TOTAL - DEFAULT_PACKET_COMPONENT_SIZE];

	// TODO: Implement serialization and deserialization of data
};

inline DataReplicationPacketComponent::DataReplicationPacketComponent()
	: PacketComponent(static_cast<int16_t>(Net::EPacketComponent::DataReplication), DEFAULT_PACKET_COMPONENT_SIZE) // Exclude size of data in the child component as it can vary
{ }