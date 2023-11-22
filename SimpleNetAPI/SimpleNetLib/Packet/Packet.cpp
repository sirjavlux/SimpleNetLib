#include "Packet.h"

#include "PacketComponent.h"

Packet::Packet(const EPacketHandlingType InPacketHandlingType)
    : packetHandlingType_(InPacketHandlingType), packetIdentifier_(GenerateIdentifier())
{
    SecureZeroMemory(data_, sizeof(data_));
}

bool Packet::IsValid() const
{
    return packetIdentifier_ > INT32_MIN;
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
    if (packetDataIter_ + componentSize > NET_BUFFER_SIZE_TOTAL)
    {
        return EAddComponentResult::SizeOutOfBounds;
    }
    
    std::memcpy(&data_[packetDataIter_], &InPacketComponent, componentSize);
    
    packetDataIter_ += componentSize;
    
    return EAddComponentResult::Success;
}

void Packet::GetComponents(std::vector<PacketComponent*>& OutComponents)
{
    int iterator = 0;
    ExtractComponent(OutComponents, iterator);
}

void Packet::ExtractComponent(std::vector<PacketComponent*>& OutComponents, int& Iterator)
{
    if (data_[Iterator] == '\0')
    {
        return;    
    }

    PacketComponent* componentExtracted = reinterpret_cast<PacketComponent*>(&data_[Iterator]);
    const uint16_t componentSize = componentExtracted->GetSize();
    OutComponents.push_back(componentExtracted);
    
    Iterator += componentSize;
    ExtractComponent(OutComponents, Iterator);
}
