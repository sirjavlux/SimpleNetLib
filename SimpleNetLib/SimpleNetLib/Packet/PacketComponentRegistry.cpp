#include "PacketComponentRegistry.h"

const PacketComponentAssociatedData* Net::PacketComponentRegistry::FetchPacketComponentAssociatedData(const uint16_t InIdentifier)
{
	return packetAssociatedData_.find(InIdentifier) != packetAssociatedData_.end() ? &packetAssociatedData_.at(InIdentifier) : nullptr;
}
