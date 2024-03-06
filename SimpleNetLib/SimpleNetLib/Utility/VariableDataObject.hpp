#pragma once

#include "../NetIncludes.h"

template<int DataSize>
struct VariableDataObject
{
	void Begin() const;
	void Reset();
	
	uint8_t data[DataSize];

	mutable uint16_t dataIter = 0;
	
	template<typename T, int ArraySize = 1>
	uint16_t operator<<(const T& InData)
	{
		const int dataSize = sizeof(T) * ArraySize;
		
		// Ensure that there's enough space in the data buffer
		if (dataIter + dataSize > DataSize)
		{
			throw std::runtime_error("Insufficient space in the data buffer");
		}
	
		std::memcpy(&data[dataIter], &InData, dataSize);
		dataIter += dataSize;

		return dataSize;
	}
	
	template<typename T, int ArraySize = 1>
	friend T operator<<(T& OutResult, VariableDataObject<DataSize>& InVariableData);
};

template<typename T, int DataSize, int ArraySize = 1>
T& operator<<(T& OutResult, const VariableDataObject<DataSize>& InVariableData)
{
	const int dataSize = sizeof(T) * ArraySize;

	// Ensure that there's enough space in the data buffer
	if (InVariableData.dataIter + dataSize > DataSize)
	{
		throw std::runtime_error("Insufficient space in the data buffer");
	}
	
	std::memcpy(&OutResult, &InVariableData.data[InVariableData.dataIter], dataSize);
	InVariableData.dataIter += dataSize;

	return OutResult;
}

template <int DataSize>
void VariableDataObject<DataSize>::Begin() const
{
	dataIter = 0;
}

template<int DataSize>
void VariableDataObject<DataSize>::Reset()
{
	SecureZeroMemory(&data, sizeof(data));
	dataIter = 0;
}
