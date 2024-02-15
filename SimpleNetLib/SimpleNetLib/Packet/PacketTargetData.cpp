#include "PacketManager.h"

#include "PacketTargetData.h"

#include "CorePacketComponents/ReturnAckComponent.hpp"

void Net::PacketTargetData::AddPacketComponentToSend(const std::shared_ptr<PacketComponent>& InComponent)
{
  const PacketComponentAssociatedData* packetComponentSettings = PacketManager::Get()->FetchPacketAssociatedData(InComponent->GetIdentifier());
  if (packetComponentSettings == nullptr)
  {
    return;
  }
  
  const PacketFrequencyData frequencyData = { packetComponentSettings->packetFrequency, packetComponentSettings->handlingType }; 
  if (packetComponentsToSendAtCertainFrequency_.find(frequencyData) == packetComponentsToSendAtCertainFrequency_.end())
  {
    packetComponentsToSendAtCertainFrequency_.insert({ frequencyData, {} });
  }
    
  std::vector<std::shared_ptr<PacketComponent>>& components = packetComponentsToSendAtCertainFrequency_.at(frequencyData);
  components.push_back(InComponent); // TODO: Needs to add way of checking overriding components
}

void Net::PacketTargetData::PushAckPacketIfContainingData(const PacketFrequencyData& PacketFrequencyData, const Packet& Packet)
{
  if (Packet.IsEmpty())
  {
    return;
  }
        
  ackPacketsNotReturned_.insert({ Packet.GetIdentifier(), { PacketFrequencyData, Packet } });

  std::cout << "Added ack packet " << Packet.GetIdentifier() << "\n";
}

void Net::PacketTargetData::RemoveReturnedPacket(const int32_t InIdentifier)
{
  ackPacketsNotReturned_.erase(InIdentifier);
}
