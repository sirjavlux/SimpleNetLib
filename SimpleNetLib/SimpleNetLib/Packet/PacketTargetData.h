#pragma once

#include "Packet.h"
#include "PacketComponent.h"

struct PacketToSendData
{
    const std::vector<std::shared_ptr<Net::PacketComponent>>& GetComponents() const
    {
      return components_;
    }

    void AddComponent(const std::shared_ptr<Net::PacketComponent>& InComponent, const PacketComponentAssociatedData& InAssociatedData);
  
    void RemoveComponents(int InIndex, int InAmount);
    void RemoveComponent(int InIndex);
  
private:
    std::map<uint16_t, uint16_t> componentTypeIndexMap_;
    std::vector<std::shared_ptr<Net::PacketComponent>> components_;
};

namespace Net
{
class PacketTargetData
{
public:
  explicit PacketTargetData() = default;

  void AddPacketComponentToSend(const std::shared_ptr<PacketComponent>& InComponent);
  
  std::map<PacketFrequencyData, PacketToSendData>& GetPacketComponentsToSend()
  { return packetComponentsToSendAtCertainFrequency_; }
  
  void PushAckPacketIfContainingData(const PacketFrequencyData& PacketFrequencyData, const Packet& Packet);
    
  const std::map<uint32_t, std::pair<PacketFrequencyData, Packet>>& GetPacketsNotReturned() const
  { return ackPacketsNotReturned_; }
  
  void RemoveReturnedPacket(uint32_t InIdentifier);

  static uint8_t FromPacketComponentSendFrequencySecondsToTicks(float InFrequencySeconds);
  
private:
  std::map<uint32_t, std::pair<PacketFrequencyData, Packet>> ackPacketsNotReturned_;
  std::map<PacketFrequencyData, PacketToSendData> packetComponentsToSendAtCertainFrequency_;
};
}
