#pragma once

#include "../../Utility/VariableDataObject.hpp"

class NET_LIB_EXPORT DataReplicationPacketComponent : public Net::PacketComponent
{
public:
	explicit DataReplicationPacketComponent();

	void Reset();
	
	uint16_t identifierData;
	uint16_t identifierDataSecond;

	VariableDataObject<REPLICATION_DATA_SIZE> variableDataObject;

	template<typename T>
	bool operator<<(const T& InData);
	
	template<typename T>
	friend T operator<<(T& OutResult, DataReplicationPacketComponent& InComponent);
};

template <typename T>
bool DataReplicationPacketComponent::operator<<(const T& InData)
{
	const uint16_t size = variableDataObject << InData;
	sizeData_ += size;
	
	return size > 0;
}

template<typename T>
T operator<<(T& OutResult, const DataReplicationPacketComponent& InComponent)
{
	OutResult << InComponent.variableDataObject;

	return OutResult;
}

inline DataReplicationPacketComponent::DataReplicationPacketComponent()
	: PacketComponent(static_cast<int16_t>(Net::EPacketComponent::DataReplication), REPLICATION_COMPONENT_SIZE_EMPTY) // Exclude size of data in the child component as it can vary
{ }

inline void DataReplicationPacketComponent::Reset()
{
	variableDataObject.Reset();
	sizeData_ = REPLICATION_COMPONENT_SIZE_EMPTY;
}
