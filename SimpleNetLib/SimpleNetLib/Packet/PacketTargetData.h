#pragma once

#include "Packet.h"
#include "PacketComponent.h"

struct PacketResendData
{
	void AddPacket(const Net::Packet& InPacket);
	void RemovePacket(uint32_t InIdentifier);
	
	void UpdateComponentAmountToSend(const PacketFrequencyData& InFrequency)
	{
		acksToResendEachFrame_ = static_cast<int>(packets_.size() / InFrequency.frequency);
		if (acksToResendEachFrame_ < 1 && !packets_.empty())
		{
			acksToResendEachFrame_ = 1;
		}
		
		packetIterator_ = 0;
	}

	void ResendPackets(const sockaddr_storage& InTarget);
	
private:
	std::unordered_map<uint32_t, Net::Packet> packets_;
	int packetIterator_ = 0;
	int acksToResendEachFrame_ = 0;
};

struct PacketToSendData
{
    const std::map<uint32_t, std::shared_ptr<Net::PacketComponent>>& GetComponents() const
    {
      return components_;
    }

    void AddComponent(const std::shared_ptr<Net::PacketComponent>& InComponent, const PacketComponentAssociatedData& InAssociatedData);

	void RemoveComponentByOverrideData(uint16_t InOverrideData);
	void RemoveComponentBySendDataId(uint32_t InSendDataId);
	
	bool TryOverrideExistingComponent(const std::shared_ptr<Net::PacketComponent>& InComponent, const PacketComponentAssociatedData& InAssociatedData);
	
	void UpdateComponentAmountToSend(const PacketFrequencyData& InFrequency)
	{
		componentsToSendPerFrame_ = static_cast<int>(components_.size() / InFrequency.frequency);
		if (componentsToSendPerFrame_ < 1 && !components_.empty())
		{
			componentsToSendPerFrame_ = 1;
		}
	}
	
	int AmountOfComponentsToSendPerFrame() const { return componentsToSendPerFrame_; }

	uint32_t GetOverrideObjectByData(uint16_t InOverrideData) const;

private:
	int componentsToSendPerFrame_ = 0;

	uint32_t componentSendDataIdIter_ = 0;
	
	// All Components
    //std::vector<std::shared_ptr<Net::PacketComponent>> components_;
	// Packet Component SendData ID : Packet Component
	std::map<uint32_t, std::shared_ptr<Net::PacketComponent>> components_;
	
	// Override Data : Packet Component SendData ID
	std::unordered_map<uint16_t, uint32_t> overrideComponents_;
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

	static uint16_t FromPacketComponentSendFrequencySecondsToTicks(float InFrequencySeconds);
	static uint16_t GetLodedFrequency(const PacketComponentAssociatedData* InAssociatedData, float InDistanceSqr);
	
private:
	void AddPacketComponentToSend(const std::shared_ptr<PacketComponent>& InComponent,
		const PacketFrequencyData& InFrequencyData, const PacketComponentAssociatedData* InPacketComponentSettings);

	std::unordered_map<uint32_t, PacketFrequencyData> ackPacketsFrequencyMap_;
	std::unordered_map<PacketFrequencyData, PacketResendData> ackPacketsNotReturned_;
	std::unordered_map<PacketFrequencyData, PacketToSendData> packetComponentsToSendAtCertainFrequency_;
};
}
