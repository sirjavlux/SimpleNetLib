#pragma once

#include <chrono>

#include "SequenceNumberBitmap.hpp"
#include "../Utility/NetPosition.h"
#include "../Utility/NetOperatorOverloads.hpp"

namespace Net
{
struct NetTarget
{
    explicit NetTarget(const sockaddr_storage& InAddress) : address(InAddress), lastTimeReceivedNetEvent(std::chrono::steady_clock::now()) { }
    NetTarget() : NetTarget(sockaddr_storage()) { }

    sockaddr_storage address;

    std::chrono::steady_clock::time_point lastTimeReceivedNetEvent;
    
    bool operator==(const sockaddr_storage& InNetTarget) const
    {
        return InNetTarget == this->address;
    }

    bool operator!=(const sockaddr_storage& InNetTarget) const
    {
        return !(InNetTarget == this->address);
    }
    
    bool operator==(const NetTarget& InNetTarget) const
    {
        return InNetTarget.address == this->address;
    }
    
    bool operator!=(const NetTarget& InNetTarget) const
    {
        return !(*this == InNetTarget);
    }
    
    SequenceNumberBitmap sequenceNumberBitmap;

    NetUtility::NetPosition netCullingPosition;
    
    friend class NetConnectionHandler;
};
}

namespace Net
{
inline bool operator<(const NetTarget& InLhs, const NetTarget& InRhs)
{
    return InLhs.address < InRhs.address;
}

inline bool operator==(const std::pair<const sockaddr_in, NetTarget>& InLhs, const std::pair<const sockaddr_in, NetTarget>& InRhs)
{
    return InLhs.first == InRhs.first;
}
}