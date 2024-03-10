#include "Packet.h"

#include "PacketComponent.h"
#include "PacketComponentRegistry.h"
#include "../SimpleNetLibCore.h"

namespace Net
{
Packet::Packet(const Packet& InPacket)
{
    *this = InPacket;
}

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

uint32_t Packet::GenerateIdentifier()
{
    static uint32_t identifierIter = 1;
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

void Packet::GetComponents(std::vector<const PacketComponent*>& OutComponents) const
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

uint16_t Packet::CalculateAndUpdateCheckSum()
{
    checkSum_ = CalculateCheckSum();
    return checkSum_;
}

uint16_t Packet::CalculateCheckSum() const
{
    uint16_t sumResult = 0;
    sumResult += static_cast<uint16_t>(packetIdentifier_);
    sumResult += static_cast<uint16_t>(packetHandlingType_);
    
    for (uint16_t bufferIter = 0; bufferIter < NET_PACKET_COMPONENT_DATA_SIZE_TOTAL; bufferIter += 4)
    {
        sumResult += *reinterpret_cast<const uint16_t*>(&data_[bufferIter]);
    }

    return sumResult;
}

uint16_t Packet::GetCheckSum() const
{
    return checkSum_;
}

Packet& Packet::operator=(const Packet& InPacket)
{
    this->packetIdentifier_ = InPacket.packetIdentifier_;
    this->checkSum_ = InPacket.checkSum_;
    this->packetHandlingType_ = InPacket.packetHandlingType_;
    this->packetDataIter_ = InPacket.packetDataIter_;
    
    std::memcpy(this->data_, InPacket.data_, NET_PACKET_COMPONENT_DATA_SIZE_TOTAL);

    return *this;
}

void Packet::ExtractComponent(std::vector<const PacketComponent*>& OutComponents, int& Iterator) const
{
    while (Iterator < NET_PACKET_COMPONENT_DATA_SIZE_TOTAL)
    {
        // Check if the component data is null-terminated
        if (data_[Iterator] == '\0')
        {
            break;
        }
        
        const uint16_t componentNetworkSentSize = *reinterpret_cast<const uint16_t*>(&data_[Iterator]);
        
        // Check if the component size exceeds the remaining buffer size
        if (componentNetworkSentSize == 0 || Iterator + componentNetworkSentSize > NET_PACKET_COMPONENT_DATA_SIZE_TOTAL)
        {
            break;
        }

        const uint16_t identifier = *reinterpret_cast<const uint16_t*>(&data_[Iterator + 2]);
        const uint16_t totalComponentObjectSize = SimpleNetLibCore::Get()->GetPacketComponentRegistry()->FetchPacketComponentAssociatedData(identifier)->componentObjectTotalSize;
        
        uint8_t* packetComponentAllocatedMemory = new uint8_t[totalComponentObjectSize];
        std::memcpy(packetComponentAllocatedMemory, &data_[Iterator], componentNetworkSentSize);
        
        const PacketComponent* componentExtracted = reinterpret_cast<const PacketComponent*>(packetComponentAllocatedMemory);
        
        OutComponents.push_back(componentExtracted);
        Iterator += componentNetworkSentSize;
    }
}
}
