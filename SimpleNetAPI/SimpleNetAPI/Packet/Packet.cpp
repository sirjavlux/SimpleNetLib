#include "Packet.h"

#include "PacketComponent.h"

Packet::Packet(const EPacketPacketType InPacketType, const EPacketHandlingType InPacketHandlingType)
    : packetType_(InPacketType), packetHandlingType_(InPacketHandlingType), packetIdentifier_(GenerateIdentifier())
{
    SecureZeroMemory(data_, sizeof(data_));
}

bool Packet::IsValid() const
{
    return packetType_ != EPacketPacketType::None && packetIdentifier_ > INT32_MIN;
}

int32_t Packet::GenerateIdentifier()
{
    static int32_t identifierIter = INT32_MIN;
    ++identifierIter; // Add before assignment to make use of invalid identifiers of "INT32_MIN"
    return identifierIter;
}

EAddComponentResult Packet::AddComponent(const PacketComponent& InPacketComponent)
{
    if (!InPacketComponent.IsValid())
    {
        return EAddComponentResult::InvalidComponent;
    }
    
    const uint16_t componentSize = InPacketComponent.GetSize();
    const uint16_t totalMemSize = sizeof(componentSize) + componentSize;
    if (packetDataIter_ + totalMemSize >= PACKET_COMPONENT_TOTAL_SPACE)
    {
        return EAddComponentResult::SizeOutOfBounds;
    }

    const PacketComponentData componentData = { componentSize, &InPacketComponent };
    memmove(&data_[packetDataIter_], &componentData, totalMemSize);
    
    packetDataIter_ += totalMemSize;
    
    return EAddComponentResult::Success;
}