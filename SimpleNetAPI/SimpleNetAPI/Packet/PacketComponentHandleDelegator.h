#pragma once

#include "../Utility/NetTag.h"

class PacketComponent;

class PacketComponentHandleDelegator
{
public:
    void HandleComponent(const PacketComponent& InPacketComponent);
    
    void MapComponentHandleDelegate(const PacketComponent& InPacketComponent,
        const std::function<void(const PacketComponent&)>& InFunction);
    
private:
    std::map<uint16_t, std::function<void(const PacketComponent&)>> delegates_;
};
