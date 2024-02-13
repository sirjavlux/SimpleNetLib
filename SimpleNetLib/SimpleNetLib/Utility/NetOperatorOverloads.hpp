#pragma once

#include <ws2def.h>

inline bool operator<(const sockaddr_in& InLhs, const sockaddr_in& InRhs) {
    if (std::memcmp(&InLhs.sin_addr, &InRhs.sin_addr, sizeof(in_addr)) < 0)
    {
        return true;
    } 
    if (std::memcmp(&InLhs.sin_addr, &InRhs.sin_addr, sizeof(in_addr)) > 0)
    {
        return false;
    }

    return ntohs(InLhs.sin_port) < ntohs(InRhs.sin_port);
}

inline bool operator<(const sockaddr_storage& InLhs, const sockaddr_storage& InRhs) {
    if (InLhs.ss_family != InRhs.ss_family)
    {
        return InLhs.ss_family < InRhs.ss_family;
    }

    if (InLhs.ss_family == AF_INET)
    {
        const sockaddr_in* lAddr = reinterpret_cast<const sockaddr_in*>(&InLhs);
        const sockaddr_in* rAddr = reinterpret_cast<const sockaddr_in*>(&InRhs);

        return *lAddr < *rAddr;
    } 
    if (InLhs.ss_family == AF_INET6)
    {
        const sockaddr_in6* lAddr = reinterpret_cast<const sockaddr_in6*>(&InLhs);
        const sockaddr_in6* rAddr = reinterpret_cast<const sockaddr_in6*>(&InRhs);

        if (std::memcmp(&lAddr->sin6_addr, &rAddr->sin6_addr, sizeof(in6_addr)) < 0)
        {
            return true;
        }
        if (std::memcmp(&lAddr->sin6_addr, &rAddr->sin6_addr, sizeof(in6_addr)) > 0)
        {
            return false;
        }

        return ntohs(lAddr->sin6_port) < ntohs(rAddr->sin6_port);
    }

    return false;
}

inline bool operator==(const sockaddr_in& InLhs, const sockaddr_in& InRhs)
{
    return InLhs.sin_port == InRhs.sin_port &&
        std::memcmp(&InLhs.sin_addr, &InRhs.sin_addr, sizeof(in_addr)) == 0;
}

inline bool operator==(const sockaddr_storage& InLhs, const sockaddr_storage& InRhs)
{
    if (InLhs.ss_family != InRhs.ss_family)
    {
        return false;
    }

    if (InLhs.ss_family == AF_INET)
    {
        const sockaddr_in* lAddr = reinterpret_cast<const sockaddr_in*>(&InLhs);
        const sockaddr_in* rAddr = reinterpret_cast<const sockaddr_in*>(&InRhs);
        
        return *lAddr == *rAddr;
    } 
    if (InLhs.ss_family == AF_INET6)
    {
        const sockaddr_in6* lAddr = reinterpret_cast<const sockaddr_in6*>(&InLhs);
        const sockaddr_in6* rAddr = reinterpret_cast<const sockaddr_in6*>(&InRhs);
        
        return lAddr->sin6_port == rAddr->sin6_port &&
            std::memcmp(&lAddr->sin6_addr, &rAddr->sin6_addr, sizeof(in6_addr)) == 0;
    }

    return false;
}