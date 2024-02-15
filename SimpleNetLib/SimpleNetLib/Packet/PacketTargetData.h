#pragma once

#include "Packet.h"
#include "PacketComponent.h"

namespace Net
{
class PacketTargetData
{
public:
  explicit PacketTargetData() = default;

  void AddPacketComponentToSend(const std::shared_ptr<PacketComponent>& InComponent);
  
  std::map<PacketFrequencyData, std::vector<std::shared_ptr<PacketComponent>>>& GetPacketComponentsToSend()
  { return packetComponentsToSendAtCertainFrequency_; }
  
  void PushAckPacketIfContainingData(const PacketFrequencyData& PacketFrequencyData, const Packet& Packet);
    
  const std::map<int32_t, std::pair<PacketFrequencyData, Packet>>& GetPacketsNotReturned() const
  { return ackPacketsNotReturned_; }
  
  void RemoveReturnedPacket(int32_t InIdentifier);
    
private:
  std::map<int32_t, std::pair<PacketFrequencyData, Packet>> ackPacketsNotReturned_;
  std::map<PacketFrequencyData, std::vector<std::shared_ptr<PacketComponent>>> packetComponentsToSendAtCertainFrequency_;
};
}
