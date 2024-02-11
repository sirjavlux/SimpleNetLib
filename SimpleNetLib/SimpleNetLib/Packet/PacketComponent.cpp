#include "PacketComponent.h"

namespace Net
{
PacketComponent::PacketComponent(const int16_t InIdentifier, const uint16_t InSize, const bool InShouldOverrideQueuedComponent)
{
    identifier_ = InIdentifier;
    componentData_ = InSize;
    if (InShouldOverrideQueuedComponent)
    {
        // Flip last bit if should override
        componentData_ = componentData_ | (static_cast<uint16_t>(1) << 10);
    }

    //std::cout << "Size " << GetSize() << " " << identifier_ << "\n";
    //std::cout << "Should Override " << (ShouldOverrideQueuedComponent() ? "true" : "false") << " " << identifier_ << "\n";
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
    }
    return *this;
}
}