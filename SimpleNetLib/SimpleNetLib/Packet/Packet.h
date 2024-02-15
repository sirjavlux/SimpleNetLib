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
    Packet(EPacketHandlingType InPacketHandlingType);
    Packet(const char* InBuffer, const int InBytesReceived);
    
    bool IsValid() const;
    bool IsEmpty() const;
    
    EAddComponentResult AddComponent(const PacketComponent& InPacketComponent);
    
    void GetComponents(std::vector<const PacketComponent*>& OutComponents) const;

    void Reset();

    const char* GetData() const { return &data_[0]; }

    int32_t GetIdentifier() const { return packetIdentifier_; }

    EPacketHandlingType GetPacketType() const { return packetHandlingType_; }
    
private:
    void ExtractComponent(std::vector<const PacketComponent*>& OutComponents, int& Iterator) const;

    int32_t packetIdentifier_ = INT32_MIN;

    EPacketHandlingType packetHandlingType_ = EPacketHandlingType::None;
    
    static int32_t GenerateIdentifier();
    
    char data_[NET_PACKET_COMPONENT_DATA_SIZE_TOTAL];

    // Data bypassing the packet size limit
    // THIS DATA IS NOT INCLUDED IN PACKET NET TRANSFERS
    uint16_t packetDataIter_ = 0;
};
}