#pragma once

#include "../NetIncludes.h"

struct PacketFrequencyData
{
	uint8_t frequency = 30;
	float ackFrequencyMultiplier = 1.f;
	EPacketHandlingType handlingType;
  
	bool operator==(const PacketFrequencyData& InOther) const
	{
		return frequency == InOther.frequency && handlingType == InOther.handlingType && ackFrequencyMultiplier == InOther.ackFrequencyMultiplier;
	}
    
	bool operator<(const PacketFrequencyData& InOther) const
	{
		if (frequency != InOther.frequency)
			return frequency < InOther.frequency;
		return handlingType < InOther.handlingType;
	}
};

namespace std
{
template<> struct equal_to<PacketFrequencyData>
{
	bool operator()(const PacketFrequencyData& InLhs, const PacketFrequencyData& InRhs) const
	{
		return InLhs == InRhs;
	}
};

template<> struct hash<PacketFrequencyData>
{
	std::size_t operator()(const PacketFrequencyData& InData) const
	{
		std::size_t hashResult = 0;
		hash_combine(hashResult, InData.frequency);
		hash_combine(hashResult, InData.handlingType);
		hash_combine(hashResult, InData.ackFrequencyMultiplier);
		return hashResult;
	}
};
}

struct PacketComponentAssociatedData
{
	bool shouldOverrideOldWaitingComponent = false;

	// Send delay in seconds/60 fps fixed
	float packetFrequencySeconds = 0.5f;
	float packetFrequencyAckResendMultiplier = 2.f;
	
	EPacketHandlingType handlingType;

	// The distance at which the packet component will not be sent at if exceeded.
	// -1 == Will always send
	float distanceToCullPacketComponentAt = -1.f;
	
	// first -> distance : second -> frequency
	std::vector<std::pair<float, float>> packetLodFrequencies;

	static bool Compare(const std::pair<float, float>& A, const std::pair<float, float>& B)
	{
		return A.first < B.first;
	}
	
	void SortLodFrequencies()
	{
		std::sort(packetLodFrequencies.begin(), packetLodFrequencies.end(), Compare);
	}
};

namespace Net
{
class NET_LIB_EXPORT PacketComponent
{
public:
	PacketComponent(int16_t InIdentifier, uint16_t InSize);
	PacketComponent(const PacketComponent& InPacketComponent);
	~PacketComponent();
	
	uint16_t GetSize() const
	{
		return sizeData_;
	}
	
	bool IsValid() const
	{
		return GetSize() >= DEFAULT_PACKET_COMPONENT_SIZE && GetSize() <= NET_PACKET_COMPONENT_DATA_SIZE_TOTAL && identifier_ > -1;
	}
	
	int16_t GetIdentifier() const { return identifier_; }

	bool ShouldOverrideComponent(const PacketComponent& InComponent) const;
	
	void SetOverrideDefiningData(const uint16_t InData) { overrideDefiningData_ = static_cast<int16_t>(InData); }
	int16_t GetOverrideDefiningData() const { return overrideDefiningData_; }
	
	bool IsOverridingDefiningDataSet() const { return overrideDefiningData_ > 0; }
	void ResetOverridingDefiningData() { overrideDefiningData_ = -1; }
	
	bool operator!=(const PacketComponent& InPacketComponent) const;
	bool operator==(const PacketComponent& InPacketComponent) const;
	
	PacketComponent& operator=(const PacketComponent& InPacketComponent);
	
protected:
	uint16_t sizeData_ = 0;
private:
	int16_t identifier_ = -1; // Valid identifier can't be negative
	int16_t overrideDefiningData_ = -1;
};
}