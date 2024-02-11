#pragma once

namespace NetUtility
{
    inline sockaddr_storage RetrieveStorageFromIPv4Address(const sockaddr_in& InAddress)
    {
        sockaddr_storage sockAddrStorage = {};
        std::memcpy(&sockAddrStorage, &InAddress, sizeof(InAddress));
        
        return sockAddrStorage;
    }

    inline sockaddr_in RetrieveIPv4AddressFromStorage(const sockaddr_storage& InAddress)
    {
        assert(InAddress.ss_family == AF_INET);
        const sockaddr_in* ipv4Addr = reinterpret_cast<const sockaddr_in*>(&InAddress);

        char ipStr[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &(ipv4Addr->sin_addr), ipStr, INET_ADDRSTRLEN);
        
        return *ipv4Addr;
    }

    inline sockaddr_in6 RetrieveIPv6AddressFromStorage(const sockaddr_storage& InAddress)
    {
        assert(InAddress.ss_family == AF_INET6);
        const sockaddr_in6* ipv6Addr = reinterpret_cast<const sockaddr_in6*>(&InAddress);

        char ipStr[INET6_ADDRSTRLEN];
        inet_ntop(AF_INET6, &(ipv6Addr->sin6_addr), ipStr, INET6_ADDRSTRLEN);

        return *ipv6Addr;
    }
}