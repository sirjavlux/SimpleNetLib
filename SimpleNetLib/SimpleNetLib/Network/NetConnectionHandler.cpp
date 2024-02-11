#include "NetConnectionHandler.h"

bool NetConnectionHandler::ContainsConnection(const sockaddr_storage& InAddress)
{
    mutexLock_.lock();

    const bool bWasFound = std::find(connections_.begin(), connections_.end(), InAddress) != connections_.end();
    
    mutexLock_.unlock();

    return bWasFound;
}

void NetConnectionHandler::RemoveConnection(const sockaddr_storage& InAddress)
{
    mutexLock_.lock();

    connections_.erase(std::find(connections_.begin(), connections_.end(), InAddress));
    
    mutexLock_.unlock();
}

void NetConnectionHandler::AddConnection(const sockaddr_storage& InAddress)
{
    if (ContainsConnection(InAddress))
    {
        return;    
    }
    
    mutexLock_.lock();

    connections_.emplace_back(InAddress);
    
    mutexLock_.unlock();
}

std::vector<NetTarget> NetConnectionHandler::GetCopy()
{
    mutexLock_.lock();
    
    std::vector<NetTarget> result = connections_;
    
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

void NetConnectionHandler::UpdateNetTarget(const NetTarget& InNetTarget)
{
    mutexLock_.lock();

    NetTarget* netTarget = RetrieveNetTarget(InNetTarget.address);
    if (netTarget)
    {
        *netTarget = InNetTarget;
    }
    
    mutexLock_.unlock();
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

bool NetConnectionHandler::HasPacketBeenSent(const sockaddr_storage& InAddress, const int32_t InIdentifier)
{
    bool bResult = false;
    
    mutexLock_.lock();

    const NetTarget* netTarget = RetrieveNetTarget(InAddress);
    if (netTarget)
    {
        bResult = netTarget->HasPacketBeenSent(InIdentifier);
    }
    
    mutexLock_.unlock();

    return bResult;
}

void NetConnectionHandler::UpdatePacketTracker(const sockaddr_storage& InAddress, const int32_t InIdentifier)
{
    mutexLock_.lock();

    NetTarget* netTarget = RetrieveNetTarget(InAddress);
    if (netTarget)
    {
        netTarget->UpdatePacketTracker(InIdentifier);
    }

    mutexLock_.unlock();
}

NetTarget* NetConnectionHandler::RetrieveNetTarget(const sockaddr_storage& InAddress)
{
    auto iter = std::find(connections_.begin(), connections_.end(), InAddress);
    if (iter != connections_.end())
    {
        return iter._Ptr;
    }
    return nullptr;
}
