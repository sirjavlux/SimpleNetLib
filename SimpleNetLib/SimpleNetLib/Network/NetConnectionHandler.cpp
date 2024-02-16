#include "NetConnectionHandler.h"

namespace Net
{
bool NetConnectionHandler::ContainsConnection(const sockaddr_storage& InAddress)
{
    bool bWasFound = false;
    
    mutexLock_.lock();

    if (InAddress.ss_family == AF_INET)
    {
        bWasFound = connections_.find(NetUtility::RetrieveIPv4AddressFromStorage(InAddress)) != connections_.end();
    }
    
    mutexLock_.unlock();

    return bWasFound;
}

void NetConnectionHandler::RemoveConnection(const sockaddr_storage& InAddress)
{
    mutexLock_.lock();

    if (InAddress.ss_family == AF_INET)
    {
        connections_.erase(NetUtility::RetrieveIPv4AddressFromStorage(InAddress));
    }
    
    mutexLock_.unlock();
}

void NetConnectionHandler::AddConnection(const sockaddr_storage& InAddress)
{
    if (ContainsConnection(InAddress))
    {
        return;    
    }
    
    mutexLock_.lock();

    if (InAddress.ss_family == AF_INET)
    {
        connections_.insert({ NetUtility::RetrieveIPv4AddressFromStorage(InAddress), {} });
    }
    
    mutexLock_.unlock();
}

std::unordered_map<sockaddr_in, NetTarget> NetConnectionHandler::GetCopy()
{
    mutexLock_.lock();
    
    std::unordered_map<sockaddr_in, NetTarget> result = connections_;
    
    mutexLock_.unlock();

    return result;
}

bool NetConnectionHandler::GetNetTargetCopy(const sockaddr_storage& InAddress, NetTarget& OutNetTarget)
{
    bool bWasFound = false;
    
    mutexLock_.lock();

    const NetTarget* netTarget = RetrieveNetTarget(InAddress);
    if (netTarget)
    {
        OutNetTarget = *netTarget;
        bWasFound = true;
    }
    
    mutexLock_.unlock();

    return bWasFound;
}

void NetConnectionHandler::UpdateNetTargetClock(const sockaddr_storage& InAddress)
{
    using namespace std::chrono;
    
    mutexLock_.lock();

    NetTarget* netTarget = RetrieveNetTarget(InAddress);
    if (netTarget)
    {
        netTarget->lastTimeReceivedNetEvent = steady_clock::now();
    }
    
    mutexLock_.unlock();
}

bool NetConnectionHandler::HasPacketBeenReceived(const sockaddr_storage& InAddress, const uint32_t InIdentifier)
{
    bool bResult = false;
    
    mutexLock_.lock();

    const NetTarget* netTarget = RetrieveNetTarget(InAddress);
    if (netTarget)
    {
        bResult = netTarget->sequenceNumberBitmap_.IsReceived(InIdentifier);
    }
    
    mutexLock_.unlock();
    
    return bResult;
}

void NetConnectionHandler::SetPacketMarketAsReceived(const sockaddr_storage& InAddress, const uint32_t InIdentifier)
{
    mutexLock_.lock();

    NetTarget* netTarget = RetrieveNetTarget(InAddress);
    if (netTarget)
    {
        netTarget->sequenceNumberBitmap_.MarkReceived(InIdentifier);
    }
    
    mutexLock_.unlock();
}

void NetConnectionHandler::UpdateNetCullingPosition(const sockaddr_storage& InAddress, const NetUtility::NetPosition& InPosition)
{
    mutexLock_.lock();
    NetTarget* netTarget = RetrieveNetTarget(InAddress);
    mutexLock_.unlock();
    
    if (netTarget)
    {
        netTarget->netCullingPosition_ = InPosition;
    }
}

NetUtility::NetPosition NetConnectionHandler::GetNetPosition(const sockaddr_storage& InAddress)
{
    mutexLock_.lock();
    NetTarget* netTarget = RetrieveNetTarget(InAddress);
    mutexLock_.unlock();
    
    if (netTarget)
    {
        return netTarget->netCullingPosition_;
    }
    return {};
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