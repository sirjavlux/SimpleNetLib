#pragma once

#include "../NetIncludes.h"

class NET_LIB_EXPORT PacketComponent
{
public:
	PacketComponent(const int16_t InIdentifier, const uint16_t InSize);
	PacketComponent(const PacketComponent& InPacketComponent);
	~PacketComponent();
	
	bool IsValid() const;

	uint16_t GetSize() const { return size_; }
	
	bool operator!=(const PacketComponent& InPacketComponent) const;
	bool operator==(const PacketComponent& InPacketComponent) const;
	
	PacketComponent& operator=(const PacketComponent& InPacketComponent);

private:
	uint16_t size_ = 0; // Needs to be set in constructor of inheriting objects
	int16_t identifier_ = -1; // Valid identifier can't be negative
};