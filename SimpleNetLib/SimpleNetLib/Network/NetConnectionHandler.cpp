#include "NetConnectionHandler.h"

namespace Net
{
bool NetConnectionHandler::ContainsConnection(const sockaddr_storage& InAddress) const
{
    bool bWasFound = false;
    
    if (InAddress.ss_family == AF_INET)
    {
        bWasFound = connections_.find(NetUtility::RetrieveIPv4AddressFromStorage(InAddress)) != connections_.end();
    }

    return bWasFound;
}

void NetConnectionHandler::RemoveConnection(const sockaddr_storage& InAddress)
{
    if (InAddress.ss_family == AF_INET)
    {
        connections_.erase(NetUtility::RetrieveIPv4AddressFromStorage(InAddress));
    }
}

void NetConnectionHandler::AddConnection(const sockaddr_storage& InAddress)
{
    if (ContainsConnection(InAddress))
    {
        return;    
    }

    if (InAddress.ss_family == AF_INET)
    {
        connections_.insert({ NetUtility::RetrieveIPv4AddressFromStorage(InAddress), NetTarget(InAddress) });
    }
}

void NetConnectionHandler::UpdateNetTargetClock(const sockaddr_storage& InAddress)
{
    using namespace std::chrono;
    
    NetTarget* netTarget = RetrieveNetTarget(InAddress);
    if (netTarget)
    {
        netTarget->lastTimeReceivedNetEvent = steady_clock::now();
    }
}

bool NetConnectionHandler::HasPacketBeenReceived(const sockaddr_storage& InAddress, const uint32_t InIdentifier) const
{
    bool bResult = false;
    
    const NetTarget* netTarget = GetNetTarget(InAddress);
    if (netTarget)
    {
        bResult = netTarget->sequenceNumberBitmap.IsReceived(InIdentifier);
    }
    
    return bResult;
}

void NetConnectionHandler::SetPacketMarketAsReceived(const sockaddr_storage& InAddress, const uint32_t InIdentifier)
{
    NetTarget* netTarget = RetrieveNetTarget(InAddress);
    if (netTarget)
    {
        netTarget->sequenceNumberBitmap.MarkReceived(InIdentifier);
    }
}

void NetConnectionHandler::UpdateNetCullingPosition(const sockaddr_storage& InAddress, const NetUtility::NetVector3& InPosition)
{
    NetTarget* netTarget = RetrieveNetTarget(InAddress);
    if (netTarget)
    {
        netTarget->netCullingPosition = InPosition;
    }
}

NetUtility::NetVector3 NetConnectionHandler::GetNetPosition(const sockaddr_storage& InAddress) const
{
    const NetTarget* netTarget = GetNetTarget(InAddress);
    if (netTarget)
    {
        return netTarget->netCullingPosition;
    }
    return {};
}

const NetTarget* NetConnectionHandler::GetNetTarget(const sockaddr_storage& InAddress) const
{
    if (InAddress.ss_family == AF_INET)
    {
        const sockaddr_in ipv4Address = NetUtility::RetrieveIPv4AddressFromStorage(InAddress);
        const auto iter = connections_.find(ipv4Address);
        if (iter != connections_.end())
        {
            return &iter->second;
        }
    }
    return nullptr;
}

NetTarget* NetConnectionHandler::RetrieveNetTarget(const sockaddr_storage& InAddress)
{
    if (InAddress.ss_family == AF_INET)
    {
        const sockaddr_in ipv4Address = NetUtility::RetrieveIPv4AddressFromStorage(InAddress);
        const auto iter = connections_.find(ipv4Address);
        if (iter != connections_.end())
        {
            return &iter->second;
        }
    }
    return nullptr;
}
}