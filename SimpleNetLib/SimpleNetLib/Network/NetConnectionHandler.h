#pragma once

#include "../NetIncludes.h"

namespace Net
{
class NetConnectionHandler
{
public:
  bool ContainsConnection(const sockaddr_storage& InAddress) const;
  void RemoveConnection(const sockaddr_storage& InAddress);
  void AddConnection(const sockaddr_storage& InAddress);
  
  void UpdateNetTargetClock(const sockaddr_storage& InAddress);

  bool HasPacketBeenReceived(const sockaddr_storage& InAddress, uint32_t InIdentifier) const;
  void SetPacketMarketAsReceived(const sockaddr_storage& InAddress, uint32_t InIdentifier);
  
  void UpdateNetCullingPosition(const sockaddr_storage& InAddress, const NetUtility::NetVector3& InPosition);
  NetUtility::NetVector3 GetNetPosition(const sockaddr_storage& InAddress) const;

  const std::unordered_map<sockaddr_in, NetTarget>& GetConnections() const { return connections_; }

  const NetTarget* GetNetTarget(const sockaddr_storage& InAddress) const;
  
private:
  NetTarget* RetrieveNetTarget(const sockaddr_storage& InAddress);
  
  std::unordered_map<sockaddr_in, NetTarget> connections_;
};
}