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
  
  // NetTarget functions
  bool HasPacketBeenSent(const sockaddr_storage& InAddress, int32_t InIdentifier);
  void UpdatePacketTracker(const sockaddr_storage& InAddress, int32_t InIdentifier);

  int FetchAndIncreaseComponentSendCount(const sockaddr_storage& InAddress, uint16_t ComponentIdentifier);

  void UpdateNetCullingPosition(const sockaddr_storage& InAddress, const NetUtility::NetPosition& InPosition);
  NetUtility::NetPosition GetNetPosition(const sockaddr_storage& InAddress);
  
private:
  NetTarget* RetrieveNetTarget(const sockaddr_storage& InAddress);
  
  std::mutex mutexLock_;
  std::unordered_map<sockaddr_in, NetTarget> connections_;
};
}