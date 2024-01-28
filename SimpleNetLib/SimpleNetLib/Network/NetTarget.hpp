#pragma once

#include <chrono>

struct NetTarget
{
    explicit NetTarget(const sockaddr_storage& InAddress) : address(InAddress), lastTimeReceivedNetEvent(std::chrono::steady_clock::now()) { }
    NetTarget() : NetTarget(sockaddr_storage()) { }

    sockaddr_storage address;

    std::chrono::steady_clock::time_point lastTimeReceivedNetEvent;

    bool operator==(const NetTarget& InNetTarget) const
    {
        return memcmp(&address, &InNetTarget.address, sizeof(sockaddr_storage)) == 0;
    }

    bool operator!=(const NetTarget& InNetTarget) const
    {
        return !(*this == InNetTarget);
    }
};

inline bool operator<(const sockaddr_storage& InLhs, const sockaddr_storage& InRhs)
{
    // Compare the address family
    if (InLhs.ss_family < InRhs.ss_family)
    {
        return true;
    }
    if (InLhs.ss_family > InRhs.ss_family)
    {
        return false;
    }

    // Compare the actual address bytes
    if (InLhs.ss_family == AF_INET)
    {
        const sockaddr_in* lAddr = reinterpret_cast<const sockaddr_in*>(&InLhs);
        const sockaddr_in* rAddr = reinterpret_cast<const sockaddr_in*>(&InRhs);

        return std::memcmp(&lAddr->sin_addr, &rAddr->sin_addr, sizeof(in_addr)) < 0;
    }
    if (InLhs.ss_family == AF_INET6)
    {
        const sockaddr_in6* lAddr = reinterpret_cast<const sockaddr_in6*>(&InLhs);
        const sockaddr_in6* rAddr = reinterpret_cast<const sockaddr_in6*>(&InRhs);

        return std::memcmp(&lAddr->sin6_addr, &rAddr->sin6_addr, sizeof(in6_addr)) < 0;
    }

    // Handle other address families if supported later on
    
    return false;
}

inline bool operator<(const NetTarget& InLhs, const NetTarget& InRhs)
{
    return InLhs.address < InRhs.address;
}
