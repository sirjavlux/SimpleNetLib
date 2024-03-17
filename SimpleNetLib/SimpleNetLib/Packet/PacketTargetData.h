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

	bool HasSpaceForComponent(const std::shared_ptr<Net::PacketComponent>& InComponent) const;
	bool TryOverrideExistingComponent(const std::shared_ptr<Net::PacketComponent>& InComponent, const PacketComponentAssociatedData& InAssociatedData);
	
	uint16_t GetTotalSize() const { return currentMemorySize_; }
	
private:
    std::vector<std::shared_ptr<Net::PacketComponent>> components_;
	uint16_t currentMemorySize_ = 0;
};

namespace Net
{
class PacketTargetData
{
public:
  explicit PacketTargetData() = default;

  void AddPacketComponentToSend(const std::shared_ptr<PacketComponent>& InComponent, const PacketComponentAssociatedData* InCustomAssociatedData = nullptr);
  void AddPacketComponentToSendWithLod(const std::shared_ptr<PacketComponent>& InComponent, float InDistanceSqr, const PacketComponentAssociatedData* InCustomAssociatedData = nullptr);
  
  std::unordered_map<PacketFrequencyData, PacketToSendData>& GetPacketComponentsToSend()
  { return packetComponentsToSendAtCertainFrequency_; }
  
  void AddAckPacketIfContainingData(const PacketFrequencyData& PacketFrequencyData, const Packet& Packet);
    
  const std::unordered_map<uint32_t, std::pair<PacketFrequencyData, Packet>>& GetPacketsNotReturned() const
  { return ackPacketsNotReturned_; }
  
  void RemoveReturnedPacket(uint32_t InIdentifier);

  static uint8_t FromPacketComponentSendFrequencySecondsToTicks(float InFrequencySeconds);
	static uint8_t GetLodedFrequency(const PacketComponentAssociatedData* InAssociatedData, float InDistanceSqr);
	
private:
  void AddPacketComponentToSend(const std::shared_ptr<PacketComponent>& InComponent,
      const PacketFrequencyData& InFrequencyData, const PacketComponentAssociatedData* InPacketComponentSettings);
  
  std::unordered_map<uint32_t, std::pair<PacketFrequencyData, Packet>> ackPacketsNotReturned_;
  std::unordered_map<PacketFrequencyData, PacketToSendData> packetComponentsToSendAtCertainFrequency_;
};
}
