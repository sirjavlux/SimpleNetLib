#pragma once

#include "../NetIncludes.h"

namespace Net
{
class NET_LIB_EXPORT PacketComponent
{
public:
	PacketComponent(int16_t InIdentifier, uint16_t InSize, bool InShouldOverrideQueuedComponent = false);
	PacketComponent(const PacketComponent& InPacketComponent);
	~PacketComponent();

	uint16_t GetSize() const
	{
		// Clear all bits except the size bits
		constexpr uint16_t mask = (1 << 9) - 1;
		return componentData_ & mask;
	}
	
	bool IsValid() const
	{
		return GetSize() >= DEFAULT_PACKET_COMPONENT_SIZE && GetSize() <= NET_PACKET_COMPONENT_DATA_SIZE_TOTAL && identifier_ > -1;
	}
	
	uint16_t GetIdentifier() const { return identifier_; }
	
	bool ShouldOverrideQueuedComponent() const
	{
		// Check if last bit is flipped
		return (componentData_ >> 10) & static_cast<uint16_t>(1);
	}
	
	bool operator!=(const PacketComponent& InPacketComponent) const;
	bool operator==(const PacketComponent& InPacketComponent) const;
	
	PacketComponent& operator=(const PacketComponent& InPacketComponent);

private:
	uint16_t componentData_ = 0; // bit 10 == ShouldOverrideQueuedComponent, bit 0 - 9 == component size
	int16_t identifier_ = -1; // Valid identifier can't be negative
};
}