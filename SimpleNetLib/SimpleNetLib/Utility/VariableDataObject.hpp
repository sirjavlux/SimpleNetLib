#pragma once

#include "../Packet/PacketComponent.h"

template<int DataSize>
struct VariableDataObject
{
	void Begin();
	void Reset();
	
	uint8_t data[DataSize];

	uint16_t dataIter = 0;

	// Only handles objects of fixed sizes // TODO: 
	template<typename T>
	uint16_t operator<<(const T& InData)
	{
		if (dataIter + sizeof(InData) >= DataSize)
		{
			return 0; // TODO: Handle this case in a better way
		}

		const int dataSize = sizeof(InData);
	
		std::memcpy(&data[dataIter], &InData, dataSize);
		dataIter += dataSize;

		return dataSize;
	}
	
	// Only handles objects of fixed sizes // TODO: 
	template<typename T>
	friend T operator<<(T& OutResult, VariableDataObject& InComponent);
};

template<typename T, int DataSize>
T operator<<(T& OutResult, VariableDataObject<DataSize>& InVariableData)
{
	const int dataSize = sizeof(OutResult);
	
	std::memcpy(&OutResult, &InVariableData.data[InVariableData.dataIter], dataSize);
	InVariableData.dataIter += dataSize;

	return OutResult;
}

template <int DataSize>
void VariableDataObject<DataSize>::Begin()
{
	dataIter = 0;
}

template<int DataSize>
void VariableDataObject<DataSize>::Reset()
{
	SecureZeroMemory(&data, sizeof(data));
	dataIter = 0;
}
