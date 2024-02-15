#pragma once

#include <chrono>

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

private:
    
    bool HasPacketBeenSent(int32_t InIdentifier) const;
    void UpdatePacketTracker(int32_t InIdentifier);
    
    std::vector<int32_t> packetsMissingPriorToCurrent_; // TODO: Needs to be able to handle lost packets
    int32_t currentPacket_ = INT32_MIN;

    NetUtility::NetPosition netCullingPosition_;
    
    friend class NetConnectionHandler;
};

inline bool NetTarget::HasPacketBeenSent(const int32_t InIdentifier) const
{
    if (InIdentifier > currentPacket_)
    {
        return false;
    } 

    for (size_t i = 0; i < packetsMissingPriorToCurrent_.size(); ++i)
    {
        if (packetsMissingPriorToCurrent_[i] == InIdentifier)
        {
            return false;
        }
    }

    return true;
}

inline void NetTarget::UpdatePacketTracker(const int32_t InIdentifier)
{
    if (InIdentifier > currentPacket_)
    {
        for (int32_t i = currentPacket_ + 1; i < InIdentifier; ++i)
        {
            packetsMissingPriorToCurrent_.push_back(i);
        }
        currentPacket_ = InIdentifier;
    }
    else
    {
        size_t foundIndex = 0;
        bool bFoundIndex = false;
        for (size_t i = 0; i < packetsMissingPriorToCurrent_.size(); ++i)
        {
            if (packetsMissingPriorToCurrent_[i] == InIdentifier)
            {
                foundIndex = i;
                bFoundIndex = true;
                break;
            }
        }

        if (bFoundIndex)
        {
            packetsMissingPriorToCurrent_.erase(packetsMissingPriorToCurrent_.begin() + foundIndex);
        }
    }
}
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