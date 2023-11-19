#pragma once

#include "../NetIncludes.h"

class PacketComponent;

enum class NET_LIB_EXPORT EPacketHandlingType : uint8_t
{
    None		= 0,
    Ack			= 1,
    AckReturn	= 2,
};

enum class NET_LIB_EXPORT EPacketPacketType : uint8_t
{
    None			= 0,
    ClientToServer	= 1,
    ServerToClient	= 2,
};

enum class NET_LIB_EXPORT EAddComponentResult : uint8_t
{
    Success			    = 0,
    InvalidComponent	= 1,
    SizeOutOfBounds	    = 2,
};

struct NET_LIB_EXPORT PacketComponentData
{
    uint16_t size = 0;
    const PacketComponent* packetComponent;
};

class NET_LIB_EXPORT Packet
{
public:
    Packet(EPacketPacketType InPacketType, EPacketHandlingType InPacketHandlingType);

    bool IsValid() const;
    
    EAddComponentResult AddComponent(const PacketComponent& InPacketComponent);
    
    void GetComponents(std::vector<PacketComponent*>& OutComponents);
    
private:
    void ExtractComponent(std::vector<PacketComponent*>& OutComponents, int& Iterator);
    
    EPacketPacketType packetType_ = EPacketPacketType::None;
    EPacketHandlingType packetHandlingType_ = EPacketHandlingType::None;
    
    int8_t data_[PACKET_COMPONENT_TOTAL_SPACE];

    int32_t packetIdentifier_ = INT32_MIN;
    
    static int32_t GenerateIdentifier();

    // Data bypassing the packet size limit
    uint16_t packetDataIter_ = 0;
};
