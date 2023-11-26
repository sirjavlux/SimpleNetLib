#include "PacketComponentHandleDelegator.h"

#include "../Packet/PacketComponent.h"

void PacketComponentHandleDelegator::HandleComponent(const PacketComponent& InPacketComponent)
{
    const uint16_t identifier = InPacketComponent.GetIdentifier();
    const auto iter = delegates_.find(identifier);
    if (iter != delegates_.end())
    {
        iter->second(InPacketComponent);
    }
}
