#include "PacketComponentHandleDelegator.h"

#include "PacketComponent.h"

void PacketComponentHandleDelegator::MapComponentHandleDelegate(const PacketComponent& InPacketComponent,
    const std::function<void(const PacketComponent&)>& InFunction)
{
    delegates_.insert({InPacketComponent.GetIdentifier(), InFunction});
}

void PacketComponentHandleDelegator::HandleComponent(const PacketComponent& InPacketComponent)
{
    const uint16_t identifier = InPacketComponent.GetIdentifier();
    const auto iter = delegates_.find(identifier);
    if (iter != delegates_.end())
    {
        iter->second(InPacketComponent);
    }
}
