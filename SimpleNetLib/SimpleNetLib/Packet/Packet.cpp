#include "Packet.h"

#include "PacketComponent.h"

Packet::Packet(const EPacketHandlingType InPacketHandlingType)
    : packetHandlingType_(InPacketHandlingType), packetIdentifier_(GenerateIdentifier())
{
    SecureZeroMemory(data_, sizeof(data_));
}
Packet::Packet(const char* InBuffer, const int InBytesReceived)
{
    std::memcpy(this, InBuffer, InBytesReceived);
}

bool Packet::IsValid() const
{
    return packetIdentifier_ > INT32_MIN;
}
bool Packet::IsEmpty() const
{
    return data_[0] == '\0';
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
    if (packetDataIter_ + componentSize > NET_PACKET_COMPONENT_DATA_SIZE_TOTAL)
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
void Packet::Reset()
{
    packetIdentifier_ = GenerateIdentifier();
    SecureZeroMemory(data_, sizeof(data_));
    packetDataIter_ = 0;
}

void Packet::ExtractComponent(std::vector<PacketComponent*>& OutComponents, int& Iterator)
{
    if (NET_PACKET_COMPONENT_DATA_SIZE_TOTAL <= Iterator || data_[Iterator] == '\0')
    {
        return;    
    }

    PacketComponent* componentExtracted = reinterpret_cast<PacketComponent*>(&data_[Iterator]);
    const uint16_t componentSize = componentExtracted->GetSize();
    OutComponents.push_back(componentExtracted);
    
    Iterator += componentSize;
    ExtractComponent(OutComponents, Iterator);
}
