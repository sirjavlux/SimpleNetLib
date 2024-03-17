#include "PacketManager.h"

#include "PacketTargetData.h"

#include "CorePacketComponents/ReturnAckComponent.hpp"

#include <memory>

void Net::PacketTargetData::AddPacketComponentToSend(const std::shared_ptr<PacketComponent>& InComponent, const PacketComponentAssociatedData* InCustomAssociatedData)
{
  const uint16_t componentIdentifier = InComponent->GetIdentifier();
  const PacketComponentAssociatedData* packetComponentSettings = SimpleNetLibCore::Get()->GetPacketComponentRegistry()->FetchPacketComponentAssociatedData(componentIdentifier);
  if (packetComponentSettings == nullptr)
  {
    return;
  }

  const float frequencyToSendAt = InCustomAssociatedData != nullptr ? InCustomAssociatedData->packetFrequencySeconds : packetComponentSettings->packetFrequencySeconds;

  const PacketFrequencyData frequencyData = { FromPacketComponentSendFrequencySecondsToTicks(frequencyToSendAt),
    packetComponentSettings->packetFrequencyAckResendMultiplier, packetComponentSettings->handlingType }; 
  
  AddPacketComponentToSend(InComponent, frequencyData, packetComponentSettings);
}

void Net::PacketTargetData::AddPacketComponentToSendWithLod(const std::shared_ptr<PacketComponent>& InComponent, const float InDistanceSqr, const PacketComponentAssociatedData* InCustomAssociatedData)
{
  const uint16_t componentIdentifier = InComponent->GetIdentifier();
  const PacketComponentAssociatedData* packetComponentSettings = SimpleNetLibCore::Get()->GetPacketComponentRegistry()->FetchPacketComponentAssociatedData(componentIdentifier);

  const uint8_t frequency = GetLodedFrequency(InCustomAssociatedData != nullptr ? InCustomAssociatedData : packetComponentSettings, InDistanceSqr);

  const PacketFrequencyData frequencyData = { frequency,
    packetComponentSettings->packetFrequencyAckResendMultiplier, packetComponentSettings->handlingType };
  
  AddPacketComponentToSend(InComponent, frequencyData, packetComponentSettings);
}

void Net::PacketTargetData::AddAckPacketIfContainingData(const PacketFrequencyData& PacketFrequencyData, const Packet& Packet)
{
  if (Packet.IsEmpty())
  {
    return;
  }
  
  ackPacketsNotReturned_.insert({ Packet.GetIdentifier(), { PacketFrequencyData, Packet } });
  
  //std::cout << "Added ack packet " << Packet.GetIdentifier() << "\n"; // Temporary debug
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

uint8_t Net::PacketTargetData::GetLodedFrequency(const PacketComponentAssociatedData* InAssociatedData, const float InDistanceSqr)
{
  float frequencyToSendAt = InAssociatedData->packetFrequencySeconds;
  const std::vector<std::pair<float, float>> lodFrequencies = InAssociatedData->packetLodFrequencies;
  
  size_t iter = 0;
  for (const std::pair<float, float>& frequencyData : lodFrequencies)
  {
    ++iter;
    if (InDistanceSqr > std::powf(frequencyData.first, 2.f)
      && (iter >= lodFrequencies.size() || (iter < lodFrequencies.size() && InDistanceSqr < std::powf(lodFrequencies[iter].first, 2.f))))
    {
      frequencyToSendAt = frequencyData.second;
      break;
    }
  }

  return FromPacketComponentSendFrequencySecondsToTicks(frequencyToSendAt);
}

void Net::PacketTargetData::AddPacketComponentToSend(const std::shared_ptr<PacketComponent>& InComponent,
                                                     const PacketFrequencyData& InFrequencyData, const PacketComponentAssociatedData* InPacketComponentSettings)
{
  const uint8_t relativeTick = PacketManager::Get()->GetRelativeTick();
  PacketFrequencyData newFrequency = InFrequencyData;
  newFrequency.frequency += relativeTick;
  
  if (packetComponentsToSendAtCertainFrequency_.find(InFrequencyData) == packetComponentsToSendAtCertainFrequency_.end())
  {
    packetComponentsToSendAtCertainFrequency_.insert({ InFrequencyData, {} });
  }
  
  PacketToSendData& sendData = packetComponentsToSendAtCertainFrequency_.at(InFrequencyData);
  if (newFrequency.handlingType != EPacketHandlingType::Ack && !sendData.HasSpaceForComponent(InComponent))
  {
    if (!InPacketComponentSettings->shouldOverrideOldWaitingComponent || !sendData.TryOverrideExistingComponent(InComponent, *InPacketComponentSettings))
    {
      // Try sending next tick if full
      newFrequency.frequency += 1;
      newFrequency.frequency -= relativeTick;
      
      AddPacketComponentToSend(InComponent, newFrequency, InPacketComponentSettings);
    }
  }
  else
  {
    sendData.AddComponent(InComponent, *InPacketComponentSettings);
  }
}

void PacketToSendData::AddComponent(const std::shared_ptr<Net::PacketComponent>& InComponent, const PacketComponentAssociatedData& InAssociatedData)
{
  if (InAssociatedData.shouldOverrideOldWaitingComponent && TryOverrideExistingComponent(InComponent, InAssociatedData))
  {
    return;
  }

  currentMemorySize_ += InComponent->GetSize();
  components_.push_back(InComponent);
}

void PacketToSendData::RemoveComponents(const int InIndex, const int InAmount)
{
  for (int i = 0; i < InAmount; ++i)
  {
    if (static_cast<int>(components_.size()) > InIndex)
    {
      currentMemorySize_ -= components_.at(InIndex)->GetSize();
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
    currentMemorySize_ -= components_.at(InIndex)->GetSize();
    components_.erase(components_.begin() + InIndex);
  }
}

bool PacketToSendData::HasSpaceForComponent(const std::shared_ptr<Net::PacketComponent>& InComponent) const
{
  return currentMemorySize_ + InComponent->GetSize() <= NET_PACKET_COMPONENT_DATA_SIZE_TOTAL;
}

bool PacketToSendData::TryOverrideExistingComponent(const std::shared_ptr<Net::PacketComponent>& InComponent, const PacketComponentAssociatedData& InAssociatedData)
{
  const uint16_t componentIdentifier = InComponent->GetIdentifier();
  for (auto& component : components_)
  {
    if (component->GetIdentifier() != componentIdentifier)
    {
      continue;
    }
      
    if (InComponent->ShouldOverrideComponent(*component))
    {
      // Subtract and add in case of variable size components
      currentMemorySize_ -= component->GetSize();
      currentMemorySize_ += InComponent->GetSize();
        
      component = InComponent;
      return true;
    }
  }
  return false;
}
