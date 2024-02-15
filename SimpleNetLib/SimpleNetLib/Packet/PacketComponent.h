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
	bool shouldOverrideQueuedComponent = false;
	
	float packetFrequency = 30.f;
	EPacketHandlingType handlingType;
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