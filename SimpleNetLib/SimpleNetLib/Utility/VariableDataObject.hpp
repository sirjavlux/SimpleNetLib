#pragma once

#include "../NetIncludes.h"

#define MEMBER_VARIABLE_STORAGE_DEFAULT_SIZE 2
#define VARIABLE_DATA_OBJECT_DEFAULT_SIZE 4

#define MAX_MEMBER_VARIABLE_OFFSET 1024
#define MAX_MEMBER_VARIABLE_SIZE 64

#define MEMBER_VARIABLE_OFFSET_BITS 10
#define MEMBER_VARIABLE_SIZE_BITS 6

template <typename T>
struct MemberVariableDataStorage
{
	// 10 bits for offset, 6 bits for size. Maximum size = 64, Maximum offset = 1024
	uint16_t variableOffsetAndSize = 0;
	T data;

	void SetOffset(const uint16_t InOffset)
	{
		constexpr uint16_t mask = (1 << MEMBER_VARIABLE_OFFSET_BITS) - 1;
    
		variableOffsetAndSize &= ~mask; // Clear the offset bits
		variableOffsetAndSize |= InOffset & mask; // Set the offset bits
	}

	void SetSize(const uint8_t InSize)
	{
		constexpr uint16_t mask = (1 << MEMBER_VARIABLE_SIZE_BITS) - 1;
    
		variableOffsetAndSize &= ~(mask << MEMBER_VARIABLE_OFFSET_BITS); // Clear the size bits
		variableOffsetAndSize |= (InSize & mask) << MEMBER_VARIABLE_OFFSET_BITS; // Set the size bits
	}
	
	uint16_t GetOffset() const
	{
		constexpr uint16_t mask = (1 << MEMBER_VARIABLE_OFFSET_BITS) - 1;
		const uint16_t result = variableOffsetAndSize & mask;
		
		return result;
	}
	
	uint16_t GetSize() const
	{
		uint16_t mask = (1 << MEMBER_VARIABLE_SIZE_BITS) - 1;
		mask <<= (16 - MEMBER_VARIABLE_SIZE_BITS);

		uint16_t result = variableOffsetAndSize & mask;
		result >>= (16 - MEMBER_VARIABLE_SIZE_BITS);
		
		return result;
	}
	
	bool operator==(const MemberVariableDataStorage& InDataStorage) const
	{
		return InDataStorage.variableOffsetAndSize == variableOffsetAndSize;
	}
};

// Forward declaration
template<int DataSize>
struct VariableDataObject;

// Friend function template declaration
template<typename T, int DataSize, int ArraySize>
T& operator<<(T& OutResult, const VariableDataObject<DataSize>& InVariableData);

template<int DataSize>
struct VariableDataObject
{
	void Begin() const;
	
	void ResetData();

	static uint8_t GetDefaultEmptySize() { return VARIABLE_DATA_OBJECT_DEFAULT_SIZE; }
	
	uint16_t GetTotalSize() const { return totalSize_; }
	uint16_t GetTotalSizeOfObject() const { return totalSize_ + VARIABLE_DATA_OBJECT_DEFAULT_SIZE; }

	static uint16_t GetMaxDataSize() { return DataSize - VARIABLE_DATA_OBJECT_DEFAULT_SIZE; }
	
	uint16_t GetMemberVariableDataStartIndex() const { return memberVariableDataStartIndex_; }

	const uint8_t* GetData() const { return data_; }
	
private:
	uint16_t totalSize_ = 0;
	uint16_t memberVariableDataStartIndex_ = 0; // Regular data is always stored in front of Member Variable data.
	uint8_t data_[DataSize - VARIABLE_DATA_OBJECT_DEFAULT_SIZE];
	
public:
	
	mutable uint16_t dataReadIter = 0; // Doesn't transfer by network
	
	// Serialize piece of data.
	// Needs to be deserialized in the same order as serialized.
	template<typename T, int ArraySize = 1>
	uint16_t operator<<(const T& InData)
	{
		const int dataSize = sizeof(T) * ArraySize;
		
		// Ensure that there's enough space in the data buffer
		if (totalSize_ + dataSize > DataSize)
		{
			throw std::runtime_error("Insufficient space in the data buffer");
		}

		// Move Member Variable Data to make space for new Data
		std::memmove(&data_[memberVariableDataStartIndex_ + dataSize], &data_[memberVariableDataStartIndex_], totalSize_ - memberVariableDataStartIndex_);

		// Copy in new Data
		std::memcpy(&data_[memberVariableDataStartIndex_], &InData, dataSize);
		
		memberVariableDataStartIndex_ += dataSize;
		totalSize_ += dataSize;
		
		return dataSize;
	}

	// DeSerialize piece of data.
	// Needs to be deserialized in the same order as serialized.
	template<typename T, int ArraySize = 1>
	friend T& operator<<(T& OutResult, const VariableDataObject& InVariableData);

	// Serialize piece of data.
	// Needs to be deserialized in the same order as serialized.
	template <typename DataType>
	void SerializeData(const DataType& InData, uint16_t InSize = 1);

	// DeSerialize piece of data.
	// Needs to be deserialized in the same order as serialized.
	template <typename DataType>
	uint16_t DeSerializeData(DataType& OutData, uint16_t InSize = 1) const;
	
	// Serialized class members needs to have a deterministic offset to the parent class
	template <class OwnerClass, typename MemberVariable>
	void SerializeMemberVariable(const OwnerClass& InOwnerClass, const MemberVariable& InData, uint16_t InSize = 1);

	// DeSerialized class members needs to have a deterministic offset to the parent class
	template <class OwnerClass, typename MemberVariable>
	bool DeSerializeMemberVariable(const OwnerClass& InOwnerClass, MemberVariable& OutData, uint16_t InSize = 1) const;

	VariableDataObject& operator=(const VariableDataObject& InDataObject);
	
private:
	template <class OwnerClass, typename MemberVariable>
	uint16_t GetMemberVariableOffset(const OwnerClass& InOwnerClass, const MemberVariable& InData) const;

	template <typename MemberVariable>
	void FindDataStorage(MemberVariableDataStorage<MemberVariable>& OutDataStorage, int& Iterator) const;
};

template<typename T, int DataSize, int ArraySize = 1>
T& operator<<(T& OutResult, const VariableDataObject<DataSize>& InVariableData)
{
	const int dataSize = sizeof(T) * ArraySize;
	
	// Ensure not reading the wrong data
	if (InVariableData.dataReadIter + dataSize > InVariableData.GetMemberVariableDataStartIndex())
	{
		throw std::runtime_error("Trying to read from Member Variable data");
	}
	
	std::memcpy(&OutResult, &InVariableData.GetData()[InVariableData.dataReadIter], dataSize);
	InVariableData.dataReadIter += dataSize;

	return OutResult;
}

template <int DataSize>
void VariableDataObject<DataSize>::Begin() const
{
	dataReadIter = 0;
}

template <int DataSize>
void VariableDataObject<DataSize>::ResetData()
{
	SecureZeroMemory(&data_, sizeof(data_));
	memberVariableDataStartIndex_ = 0;
	totalSize_ = 0;
	Begin();
}

template <int DataSize>
template <typename DataType>
void VariableDataObject<DataSize>::SerializeData(const DataType& InData, uint16_t InSize)
{
	this.template operator<<<DataType, DataSize, InSize>(InData);
}

template <int DataSize>
template <typename DataType>
uint16_t VariableDataObject<DataSize>::DeSerializeData(DataType& OutData, uint16_t InSize) const
{
	return operator<<<DataType, DataSize, InSize>(OutData, *this);
}

template <int DataSize>
template <class OwnerClass, typename MemberVariable>
void VariableDataObject<DataSize>::SerializeMemberVariable(const OwnerClass& InOwnerClass, const MemberVariable& InData, const uint16_t InSize)
{
	const uint16_t offset = GetMemberVariableOffset(InOwnerClass, InData);
	const uint16_t dataSize = sizeof(MemberVariable) * InSize;

	if (totalSize_ + dataSize + MEMBER_VARIABLE_STORAGE_DEFAULT_SIZE > DataSize)
	{
		throw std::runtime_error("Insufficient space in the data buffer");
	}

	MemberVariableDataStorage<MemberVariable> dataStorage;
	dataStorage.SetOffset(offset);
	dataStorage.SetSize(dataSize);
	std::memcpy(&dataStorage.data, &InData, dataSize);

	std::memcpy(&data_, &dataStorage, sizeof(dataStorage));
	
	totalSize_ += dataSize + MEMBER_VARIABLE_STORAGE_DEFAULT_SIZE;
}

template <int DataSize>
template <class OwnerClass, typename MemberVariable>
bool VariableDataObject<DataSize>::DeSerializeMemberVariable(const OwnerClass& InOwnerClass, MemberVariable& OutData, const uint16_t InSize) const
{
	const uint16_t offset = GetMemberVariableOffset(InOwnerClass, OutData);
	const uint8_t dataSize = sizeof(MemberVariable) * InSize;
	
	MemberVariableDataStorage<MemberVariable> dataStorage;
	dataStorage.SetOffset(offset);
	dataStorage.SetSize(dataSize);

	int iterator = memberVariableDataStartIndex_;
	FindDataStorage<MemberVariable>(dataStorage, iterator);

	if (iterator < GetMaxDataSize() && iterator < totalSize_)
	{
		std::memcpy(&OutData, &dataStorage.data, dataSize);
		return true;
	}
	
	return false;
}

template <int DataSize>
VariableDataObject<DataSize>& VariableDataObject<DataSize>::operator=(const VariableDataObject& InDataObject)
{
	this->dataReadIter = 0;
	this->totalSize_ = InDataObject.totalSize_;
	this->memberVariableDataStartIndex_ = InDataObject.memberVariableDataStartIndex_;
	std::memcpy(&this->data_[0], &InDataObject.data_[0], InDataObject.totalSize_);

	return *this;
}

template <int DataSize>
template <class OwnerClass, typename MemberVariable>
uint16_t VariableDataObject<DataSize>::GetMemberVariableOffset(const OwnerClass& InOwnerClass, const MemberVariable& InData) const
{
	static_assert(std::is_member_pointer<MemberVariable OwnerClass::*>::value, "MemberVariable is not a member variable of OwnerClass");
	
	// Calculate offset
	const uintptr_t ownerPtr = reinterpret_cast<uintptr_t>(&InOwnerClass);
	const uintptr_t dataPtr = reinterpret_cast<uintptr_t>(&InData);
	const uint16_t dataOffset = dataPtr - ownerPtr;

	return dataOffset;
}

template <int DataSize>
template <typename MemberVariable>
void VariableDataObject<DataSize>::FindDataStorage(MemberVariableDataStorage<MemberVariable>& OutDataStorage, int& Iterator) const
{
	while (Iterator < totalSize_)
	{
		const MemberVariableDataStorage<MemberVariable>* dataStorage = reinterpret_cast<const MemberVariableDataStorage<MemberVariable>*>(&data_[Iterator]);
		const uint8_t dataSize = dataStorage->GetSize();
		if (OutDataStorage == *dataStorage)
		{
			std::memcpy(&OutDataStorage.data, &dataStorage->data, dataSize);
			return;
		}

		Iterator += dataSize + MEMBER_VARIABLE_STORAGE_DEFAULT_SIZE;
	}
}
