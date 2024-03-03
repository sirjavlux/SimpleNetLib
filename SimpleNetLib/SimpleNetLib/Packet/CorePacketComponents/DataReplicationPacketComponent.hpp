#pragma once

#include "../PacketComponent.h"

#define REPLICATION_COMPONENT_SIZE_EMPTY (DEFAULT_PACKET_COMPONENT_SIZE + 2)
#define REPLICATION_DATA_SIZE (NET_PACKET_COMPONENT_DATA_SIZE_TOTAL - REPLICATION_COMPONENT_SIZE_EMPTY)

class NET_LIB_EXPORT DataReplicationPacketComponent : public Net::PacketComponent
{
public:
	explicit DataReplicationPacketComponent();

	void Reset();
	
	uint16_t identifierData;
	uint8_t data[REPLICATION_DATA_SIZE];

	uint16_t dataIter = 0;

	// Only handles objects of fixed sizes // TODO: 
	template<typename T>
	void operator<<(const T& InData);

	// Only handles objects of fixed sizes // TODO: 
	template<typename T>
	friend T operator<<(T& OutResult, DataReplicationPacketComponent& InComponent);
};

template <typename T>
void DataReplicationPacketComponent::operator<<(const T& InData)
{
	if (dataIter + sizeof(InData) >= REPLICATION_DATA_SIZE)
	{
		return; // TODO: Handle this case in a better way
	}

	const int dataSize = sizeof(InData);
	
	std::memcpy(&data[dataIter], &InData, dataSize);
	dataIter += dataSize;
	sizeData_ += dataSize;
}

template<typename T>
T operator<<(T& OutResult, DataReplicationPacketComponent& InComponent)
{
	const int dataSize = sizeof(OutResult);
	
	std::memcpy(&OutResult, &InComponent.data[InComponent.dataIter], dataSize);
	InComponent.dataIter += dataSize;
	InComponent.sizeData_ += dataSize;

	return OutResult;
}

inline DataReplicationPacketComponent::DataReplicationPacketComponent()
	: PacketComponent(static_cast<int16_t>(Net::EPacketComponent::DataReplication), REPLICATION_COMPONENT_SIZE_EMPTY) // Exclude size of data in the child component as it can vary
{ }

inline void DataReplicationPacketComponent::Reset()
{
	SecureZeroMemory(&data, sizeof(data));
	sizeData_ = REPLICATION_COMPONENT_SIZE_EMPTY;
	dataIter = 0;
}
