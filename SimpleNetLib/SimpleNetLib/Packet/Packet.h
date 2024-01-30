#pragma once

#include "../NetIncludes.h"

class PacketComponent;

enum class NET_LIB_EXPORT EPacketHandlingType : uint8_t
{
    None		= 0,
    Ack			= 1,
    
    Size        = 3
};

/*
enum class NET_LIB_EXPORT EPacketPacketType : uint8_t
{
    None			= 0,
    ClientToServer	= 1,
    ServerToClient	= 2,
};
*/

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
    Packet(EPacketHandlingType InPacketHandlingType);

    bool IsValid() const;
    bool IsEmpty() const;
    
    EAddComponentResult AddComponent(const PacketComponent& InPacketComponent);
    
    void GetComponents(std::vector<PacketComponent*>& OutComponents);

    void Reset();

    const char* GetData() const { return &data_[0]; }

    uint16_t GetIdentifier() const { return packetDataIter_; }

    EPacketHandlingType GetPacketType() const { return packetHandlingType_; }
    
private:
    void ExtractComponent(std::vector<PacketComponent*>& OutComponents, int& Iterator);
    
    EPacketHandlingType packetHandlingType_ = EPacketHandlingType::None;
    
    char data_[NET_BUFFER_SIZE_TOTAL];

    int32_t packetIdentifier_ = INT32_MIN;
    
    static int32_t GenerateIdentifier();

    // Data bypassing the packet size limit
    uint16_t packetDataIter_ = 0;
};
