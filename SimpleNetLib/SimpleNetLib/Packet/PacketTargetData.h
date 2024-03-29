#pragma once

#include "Packet.h"
#include "PacketComponent.h"

#define PACKETS_MAX_SEND_PER_TICK 4

struct PacketResendData
{
	void AddPacket(const Net::Packet& InPacket);
	void RemovePacket(uint32_t InIdentifier);
	
	void UpdateComponentAmountToSend()
	{
		acksToResend_ = static_cast<int>(packets_.size());
		packetIterator_ = 0;
	}

	void ResendPackets(const sockaddr_storage& InTarget);
	
private:
	std::unordered_map<uint32_t, Net::Packet> packets_;
	int packetIterator_ = 0;
	int acksToResend_ = 0;
};

struct PacketToSendData
{
    const std::vector<std::shared_ptr<Net::PacketComponent>>& GetComponents() const
    {
      return components_;
    }

    void AddComponent(const std::shared_ptr<Net::PacketComponent>& InComponent, const PacketComponentAssociatedData& InAssociatedData);
  
    void RemoveComponents(int InAmount);
    void RemoveComponent(int InIndex);
	
	bool TryOverrideExistingComponent(const std::shared_ptr<Net::PacketComponent>& InComponent, const PacketComponentAssociatedData& InAssociatedData);
	
	void UpdateComponentAmountToSend(const PacketFrequencyData& InFrequency)
	{
		componentsToSend_ = static_cast<int>(components_.size() / InFrequency.frequency);
		if (componentsToSend_ < 1 && !components_.empty())
			componentsToSend_ = 1;
	}
	
	int AmountOfComponentsToSend() const { return componentsToSend_; }
	
private:
    std::vector<std::shared_ptr<Net::PacketComponent>> components_;
	int componentsToSend_ = 0;
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
    
  std::unordered_map<PacketFrequencyData, PacketResendData>& GetPacketsNotReturned()
  { return ackPacketsNotReturned_; }
  
  void RemoveReturnedPacket(uint32_t InIdentifier);

  static uint8_t FromPacketComponentSendFrequencySecondsToTicks(float InFrequencySeconds);
	static uint8_t GetLodedFrequency(const PacketComponentAssociatedData* InAssociatedData, float InDistanceSqr);
	
private:
  void AddPacketComponentToSend(const std::shared_ptr<PacketComponent>& InComponent,
      const PacketFrequencyData& InFrequencyData, const PacketComponentAssociatedData* InPacketComponentSettings);

	std::unordered_map<uint32_t, PacketFrequencyData> ackPacketsFrequencyMap_;
  std::unordered_map<PacketFrequencyData, PacketResendData> ackPacketsNotReturned_;
  std::unordered_map<PacketFrequencyData, PacketToSendData> packetComponentsToSendAtCertainFrequency_;
};
}
