#pragma once

#include "../NetIncludes.h"

struct PacketFrequencyData
{
	uint8_t frequency = 30;
	EPacketHandlingType handlingType;
  
	bool operator==(const PacketFrequencyData& InOther) const
	{
		return frequency == InOther.frequency && handlingType == InOther.handlingType;
	}
    
	bool operator<(const PacketFrequencyData& InOther) const
	{
		if (frequency != InOther.frequency)
			return frequency < InOther.frequency;
		return handlingType < InOther.handlingType;
	}
};

struct PacketComponentAssociatedData
{
	bool shouldOverrideQueuedComponent = false; // TODO: Implement this

	// Send delay in seconds/60 fps fixed
	float packetFrequencySeconds = 0.5f;
	
	EPacketHandlingType handlingType;

	// Acknowledgement send delay in seconds/60 fps fixed
	float packetAckFrequencySeconds = 1.f; 
	
	std::map<float, float> cullingDistSqrWithFrequency; // TODO: Implement this or similar
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
	
	uint16_t GetIdentifier() const { return identifier_; }
	
	bool operator!=(const PacketComponent& InPacketComponent) const;
	bool operator==(const PacketComponent& InPacketComponent) const;
	
	PacketComponent& operator=(const PacketComponent& InPacketComponent);

private:
	uint16_t sizeData_ = 0;
	int16_t identifier_ = -1; // Valid identifier can't be negative
};
}