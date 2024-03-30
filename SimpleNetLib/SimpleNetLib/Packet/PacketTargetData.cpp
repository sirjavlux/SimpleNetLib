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

  const uint8_t frequencyToSendAt = InCustomAssociatedData != nullptr ? InCustomAssociatedData->packetFrequency : packetComponentSettings->packetFrequency;

  const PacketFrequencyData frequencyData = { frequencyToSendAt, packetComponentSettings->handlingType }; 
  
  AddPacketComponentToSend(InComponent, frequencyData, packetComponentSettings);
}

void Net::PacketTargetData::AddPacketComponentToSendWithLod(const std::shared_ptr<PacketComponent>& InComponent, const float InDistanceSqr, const PacketComponentAssociatedData* InCustomAssociatedData)
{
  const uint16_t componentIdentifier = InComponent->GetIdentifier();
  const PacketComponentAssociatedData* packetComponentSettings = SimpleNetLibCore::Get()->GetPacketComponentRegistry()->FetchPacketComponentAssociatedData(componentIdentifier);

  const uint8_t frequency = GetLodedFrequency(InCustomAssociatedData != nullptr ? InCustomAssociatedData : packetComponentSettings, InDistanceSqr);

  const PacketFrequencyData frequencyData = { frequency, packetComponentSettings->handlingType };
  
  AddPacketComponentToSend(InComponent, frequencyData, packetComponentSettings);
}

void Net::PacketTargetData::AddAckPacketIfContainingData(const PacketFrequencyData& PacketFrequencyData, const Packet& Packet)
{
  if (Packet.IsEmpty())
  {
    return;
  }
  
  if (ackPacketsNotReturned_.find(PacketFrequencyData) == ackPacketsNotReturned_.end())
  {
    ackPacketsNotReturned_.insert({ PacketFrequencyData, {} });
  }
  
  PacketResendData& sendData = ackPacketsNotReturned_.at(PacketFrequencyData);
  sendData.AddPacket(Packet);

  ackPacketsFrequencyMap_.insert({ Packet.GetIdentifier(), PacketFrequencyData });
  
  //std::cout << "Added ack packet " << Packet.GetIdentifier() << "\n"; // Temporary debug
}

void Net::PacketTargetData::RemoveReturnedPacket(const uint32_t InIdentifier)
{
  const auto& iteratorFrequency = ackPacketsFrequencyMap_.find(InIdentifier);
  if (iteratorFrequency != ackPacketsFrequencyMap_.end())
  {
    const auto& iteratorPacketData = ackPacketsNotReturned_.find(iteratorFrequency->second);
    if (iteratorPacketData != ackPacketsNotReturned_.end())
    {
      PacketResendData& resendData = iteratorPacketData->second;
      resendData.RemovePacket(InIdentifier);

      //std::cout << "Removed ack packet " << InIdentifier << "\n"; // Temporary debug
    }

    ackPacketsFrequencyMap_.erase(InIdentifier);
  }
}

uint8_t Net::PacketTargetData::FromPacketComponentSendFrequencySecondsToTicks(const float InFrequencySeconds)
{
  const uint8_t result = static_cast<uint8_t>(FIXED_UPDATES_PER_SECOND * InFrequencySeconds);
  return result <= 0 ? 1 : result;
}

uint8_t Net::PacketTargetData::GetLodedFrequency(const PacketComponentAssociatedData* InAssociatedData, const float InDistanceSqr)
{
  uint8_t frequencyToSendAt = InAssociatedData->packetFrequency;
  const std::vector<std::pair<float, uint8_t>>& lodFrequencies = InAssociatedData->packetLodFrequencies;
  
  size_t iter = 0;
  for (const std::pair<float, uint8_t>& frequencyData : lodFrequencies)
  {
    ++iter;
    if (InDistanceSqr > std::powf(frequencyData.first, 2.f)
      && (iter >= lodFrequencies.size() || (iter < lodFrequencies.size() && InDistanceSqr < std::powf(lodFrequencies[iter].first, 2.f))))
    {
      frequencyToSendAt = frequencyData.second;
      break;
    }
  }

  return frequencyToSendAt;
}

void Net::PacketTargetData::AddPacketComponentToSend(const std::shared_ptr<PacketComponent>& InComponent,
                                                     const PacketFrequencyData& InFrequencyData, const PacketComponentAssociatedData* InPacketComponentSettings)
{
  if (packetComponentsToSendAtCertainFrequency_.find(InFrequencyData) == packetComponentsToSendAtCertainFrequency_.end())
  {
    packetComponentsToSendAtCertainFrequency_.insert({ InFrequencyData, {} });
  }
  
  PacketToSendData& sendData = packetComponentsToSendAtCertainFrequency_.at(InFrequencyData);
  if (InFrequencyData.handlingType != EPacketHandlingType::Ack)
  {
    if (InPacketComponentSettings->shouldOverrideMatchingExistingComponent && sendData.TryOverrideExistingComponent(InComponent, *InPacketComponentSettings))
    {
      return;
    }
  }
  
  sendData.AddComponent(InComponent, *InPacketComponentSettings);
}

void PacketResendData::AddPacket(const Net::Packet& InPacket)
{
  packets_.insert({ InPacket.GetIdentifier(), InPacket });
}

void PacketResendData::RemovePacket(const uint32_t InIdentifier)
{
  packets_.erase(InIdentifier);
  packetIterator_ -= 1;
  acksToResend_ -= 1;
}

void PacketResendData::ResendPackets(const sockaddr_storage& InTarget)
{
  auto packetIterator = packets_.begin();
  
  const int end = packetIterator_ + PACKETS_MAX_SEND_PER_TICK > acksToResend_ ? acksToResend_ : packetIterator_ + PACKETS_MAX_SEND_PER_TICK;
  const int start = packetIterator_;

  if (start < 0)
    return;
  
  std::advance(packetIterator, start);
  
  for (int i = start; i < end; ++i)
  {
    Net::SimpleNetLibCore::Get()->GetNetHandler()->SendPacketToTarget(InTarget, packetIterator->second);
    std::advance(packetIterator, 1);
  }
}

void PacketToSendData::AddComponent(const std::shared_ptr<Net::PacketComponent>& InComponent, const PacketComponentAssociatedData& InAssociatedData)
{
  components_.push_back(InComponent);
}

void PacketToSendData::RemoveComponents(const int InAmount)
{
  for (int i = 0; i < InAmount; ++i)
  {
    RemoveComponent(0);
  }
}

void PacketToSendData::RemoveComponent(const int InIndex)
{
  if (static_cast<int>(components_.size()) > InIndex)
  {
    components_.erase(components_.begin() + InIndex);
    ++componentsToSendIterator_;
  }
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
      component = InComponent;
      return true;
    }
  }
  return false;
}
