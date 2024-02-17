#include "PacketManager.h"

#include "PacketTargetData.h"

#include "CorePacketComponents/ReturnAckComponent.hpp"

void Net::PacketTargetData::AddPacketComponentToSend(const std::shared_ptr<PacketComponent>& InComponent)
{
  const uint16_t componentIdentifier = InComponent->GetIdentifier();
  const PacketComponentAssociatedData* packetComponentSettings = PacketManager::Get()->FetchPacketComponentAssociatedData(componentIdentifier);
  if (packetComponentSettings == nullptr)
  {
    return;
  }
  
  const PacketFrequencyData frequencyData = { FromPacketComponentSendFrequencySecondsToTicks(packetComponentSettings->packetFrequencySeconds), packetComponentSettings->handlingType }; 

  AddPacketComponentToSend(InComponent, frequencyData, packetComponentSettings);
}

void Net::PacketTargetData::AddPacketComponentToSendWithLod(const std::shared_ptr<PacketComponent>& InComponent, const int InDistanceSqr)
{
  const uint16_t componentIdentifier = InComponent->GetIdentifier();
  const PacketComponentAssociatedData* packetComponentSettings = PacketManager::Get()->FetchPacketComponentAssociatedData(componentIdentifier);

  float frequencyToSendAt = packetComponentSettings->packetFrequencySeconds;
  for (const std::pair<int, float>& iter : packetComponentSettings->packetLodFrequencies)
  {
    if (InDistanceSqr < iter.first)
    {
      frequencyToSendAt = iter.second;
      break;
    }
  }

  const PacketFrequencyData frequencyData = { FromPacketComponentSendFrequencySecondsToTicks(frequencyToSendAt), packetComponentSettings->handlingType }; 

  AddPacketComponentToSend(InComponent, frequencyData, packetComponentSettings);
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

void Net::PacketTargetData::RemoveReturnedPacket(const uint32_t InIdentifier)
{
  ackPacketsNotReturned_.erase(InIdentifier);
}

uint8_t Net::PacketTargetData::FromPacketComponentSendFrequencySecondsToTicks(const float InFrequencySeconds)
{
  const uint8_t result = static_cast<uint8_t>(FIXED_UPDATES_PER_SECOND * InFrequencySeconds);
  return result <= 0 ? 1 : result;
}

void Net::PacketTargetData::AddPacketComponentToSend(const std::shared_ptr<PacketComponent>& InComponent,
    const PacketFrequencyData& InFrequencyData, const PacketComponentAssociatedData* InPacketComponentSettings)
{
  if (packetComponentsToSendAtCertainFrequency_.find(InFrequencyData) == packetComponentsToSendAtCertainFrequency_.end())
  {
    packetComponentsToSendAtCertainFrequency_.insert({ InFrequencyData, {} });
  }

  PacketToSendData& sendData = packetComponentsToSendAtCertainFrequency_.at(InFrequencyData);
  sendData.AddComponent(InComponent, *InPacketComponentSettings);
}

void PacketToSendData::AddComponent(const std::shared_ptr<Net::PacketComponent>& InComponent, const PacketComponentAssociatedData& InAssociatedData)
{
  const uint16_t componentIdentifier = InComponent->GetIdentifier();
  if (InAssociatedData.shouldOverrideOldWaitingComponent)
  {
    if (componentTypeIndexMap_.find(componentIdentifier) != componentTypeIndexMap_.end())
    {
      const uint16_t index = componentTypeIndexMap_.at(componentIdentifier);
      components_[index] = InComponent;
      return;
    }
  }
 
  componentTypeIndexMap_.insert({componentIdentifier, components_.size()});
  components_.push_back(InComponent);
}

void PacketToSendData::RemoveComponents(const int InIndex, const int InAmount)
{
  for (int i = 0; i < InAmount; ++i)
  {
    if (static_cast<int>(components_.size()) > InIndex)
    {
      RemoveComponent(InIndex);
    }
    else
    {
      return;
    }
  }
}

void PacketToSendData::RemoveComponent(const int InIndex)
{
  if (static_cast<int>(components_.size()) > InIndex)
  {
    const Net::PacketComponent& component = *components_.at(InIndex);
    if (componentTypeIndexMap_.find(component.GetIdentifier()) != componentTypeIndexMap_.end())
    {
      componentTypeIndexMap_.erase(component.GetIdentifier());
    }
    // Remove component
    components_.erase(components_.begin() + InIndex);
  }
}
