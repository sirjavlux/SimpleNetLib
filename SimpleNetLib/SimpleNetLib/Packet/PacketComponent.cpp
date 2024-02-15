#include "PacketComponent.h"

namespace Net
{
PacketComponent::PacketComponent(const int16_t InIdentifier, const uint16_t InSize)
{
    identifier_ = InIdentifier;
    sizeData_ = InSize;
}

PacketComponent::PacketComponent(const PacketComponent& InPacketComponent)
{
    *this = InPacketComponent;
}

PacketComponent::~PacketComponent() = default;

bool PacketComponent::operator!=(const PacketComponent& InPacketComponent) const
{
    return identifier_ != InPacketComponent.identifier_;
}

bool PacketComponent::operator==(const PacketComponent& InPacketComponent) const
{
    return identifier_ == InPacketComponent.identifier_;
}

PacketComponent& PacketComponent::operator=(const PacketComponent& InPacketComponent)
{
    if (*this != InPacketComponent)
    {
        identifier_ = InPacketComponent.identifier_;
        sizeData_ = InPacketComponent.sizeData_;
    }
    return *this;
}
}