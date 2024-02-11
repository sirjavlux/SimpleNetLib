#pragma once

#include <mutex>

#include "../NetIncludes.h"

class NetConnectionHandler
{
public:
    bool ContainsConnection(const sockaddr_storage& InAddress);
    void RemoveConnection(const sockaddr_storage& InAddress);
    void AddConnection(const sockaddr_storage& InAddress);

    std::vector<NetTarget> GetCopy();
    bool GetNetTargetCopy(const sockaddr_storage& InAddress, NetTarget& OutNetTarget);
  
    void UpdateNetTarget(const NetTarget& InNetTarget);
    void UpdateNetTargetClock(const sockaddr_storage& InAddress);
  
    // NetTarget functions
    bool HasPacketBeenSent(const sockaddr_storage& InAddress, const int32_t InIdentifier);
    void UpdatePacketTracker(const sockaddr_storage& InAddress, const int32_t InIdentifier);

private:
  NetTarget* RetrieveNetTarget(const sockaddr_storage& InAddress);
  
  std::mutex mutexLock_;
  std::vector<NetTarget> connections_;
};
