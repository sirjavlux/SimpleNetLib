#pragma once

#include "../PacketComponent.h"
#include "../../Utility/VariableDataObject.hpp"

class NET_LIB_EXPORT DataReplicationPacketComponent : public Net::PacketComponent
{
public:
	explicit DataReplicationPacketComponent();

	void Reset();

	void UpdateSize();
	
	uint16_t identifierDataFirst;
	uint16_t identifierDataSecond;

	VariableDataObject<REPLICATION_DATA_SIZE> variableDataObject;

	template<typename DataType>
	bool operator<<(const DataType& InData);
	
	template<typename T>
	friend T operator<<(T& OutResult, DataReplicationPacketComponent& InComponent);
};

template <typename DataType>
bool DataReplicationPacketComponent::operator<<(const DataType& InData)
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
{
	UpdateSize();
}

inline void DataReplicationPacketComponent::Reset()
{
	variableDataObject.ResetData();
	sizeData_ = REPLICATION_COMPONENT_SIZE_EMPTY;
}

inline void DataReplicationPacketComponent::UpdateSize()
{
	sizeData_ = variableDataObject.GetTotalSizeOfObject() + REPLICATION_COMPONENT_SIZE_EMPTY;
}
