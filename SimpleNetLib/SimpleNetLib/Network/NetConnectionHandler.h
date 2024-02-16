#pragma once

#include <mutex>

#include "../NetIncludes.h"

namespace Net
{
class NetConnectionHandler
{
public:
  bool ContainsConnection(const sockaddr_storage& InAddress);
  void RemoveConnection(const sockaddr_storage& InAddress);
  void AddConnection(const sockaddr_storage& InAddress);

  std::unordered_map<sockaddr_in, NetTarget> GetCopy();
  bool GetNetTargetCopy(const sockaddr_storage& InAddress, NetTarget& OutNetTarget);
  
  void UpdateNetTargetClock(const sockaddr_storage& InAddress);

  bool HasPacketBeenReceived(const sockaddr_storage& InAddress, uint32_t InIdentifier);
  void SetPacketMarketAsReceived(const sockaddr_storage& InAddress, uint32_t InIdentifier);
  
  void UpdateNetCullingPosition(const sockaddr_storage& InAddress, const NetUtility::NetPosition& InPosition);
  NetUtility::NetPosition GetNetPosition(const sockaddr_storage& InAddress);
  
private:
  NetTarget* RetrieveNetTarget(const sockaddr_storage& InAddress);
  
  std::mutex mutexLock_;
  std::unordered_map<sockaddr_in, NetTarget> connections_;
};
}