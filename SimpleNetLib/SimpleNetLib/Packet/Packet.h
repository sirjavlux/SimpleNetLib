#pragma once

#include "../NetIncludes.h"

namespace Net
{
    class PacketComponent;
}

struct NET_LIB_EXPORT PacketComponentData
{
    uint16_t size = 0;
    const Net::PacketComponent* packetComponent;
};

namespace Net
{
class NET_LIB_EXPORT Packet
{
public:
    Packet(const Packet& InPacket);
    Packet(EPacketHandlingType InPacketHandlingType);
    Packet(const char* InBuffer, int InBytesReceived);
    
    bool IsValid() const;
    bool IsEmpty() const;
    
    EAddComponentResult AddComponent(const PacketComponent& InPacketComponent);
    
    void GetComponents(std::vector<const PacketComponent*>& OutComponents) const;

    void Reset();

    const char* GetData() const { return &data_[0]; }

    uint32_t GetIdentifier() const { return packetIdentifier_; }

    EPacketHandlingType GetPacketType() const { return packetHandlingType_; }

    uint16_t CalculateAndUpdateCheckSum();
    uint16_t CalculateCheckSum() const;
    uint16_t GetCheckSum() const;

    Packet& operator=(const Packet& InPacket);
    
private:
    void ExtractComponent(std::vector<const PacketComponent*>& OutComponents, int& Iterator) const;
    
    uint32_t packetIdentifier_ = 0;
    uint16_t checkSum_ = 0; // Not included in checksum calculation
    
    EPacketHandlingType packetHandlingType_ = EPacketHandlingType::None;
    
    static uint32_t GenerateIdentifier();
    
    char padding_[1]; // Padding for ability to extract 4 bytes at a time
    char data_[NET_PACKET_COMPONENT_DATA_SIZE_TOTAL];

    // Data bypassing the packet size limit
    // THIS DATA IS NOT INCLUDED IN PACKET NET TRANSFERS
    uint16_t packetDataIter_ = 0;

    friend class PacketTargetData;
};
}