#pragma once

#include <chrono>

#include "../NetIncludes.h"

enum class ENetworkHandleType;
enum class EPacketHandlingType : uint8_t;

enum class ENetKickType : uint8_t
{
    TimeOut             = 0,
    ConnectionClosed    = 1,
};

struct NetSettings
{
    explicit NetSettings(const PCWSTR InParentServerAddress = TEXT(""), const PCWSTR InServerAddress = DEFAULT_SERVER_ADDRESS):
        parentServerAddress(InParentServerAddress),
        serverAddress(InServerAddress)
    {
    }

    PCWSTR parentServerAddress;
    u_short parentServerPort = 0; // Leave at 0 if no server connection

    // Self address if server
    PCWSTR serverAddress;
    u_short serverPort = DEFAULT_SERVER_PORT;
};

struct PacketComponentAssociatedData
{
    EPacketHandlingType handlingType;
};

struct NetTarget
{
    NetTarget(const sockaddr_storage& InAddress) : address(InAddress), lastTimeReceivedNetEvent(std::chrono::steady_clock::now()) { }
    NetTarget() = default;

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

namespace Net
{
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
