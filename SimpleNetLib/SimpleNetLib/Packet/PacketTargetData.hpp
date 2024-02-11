#pragma once

#include "../NetIncludes.h"

namespace Net
{
class PacketTargetData
{
public:
  explicit PacketTargetData()
      : regularPacket_(EPacketHandlingType::None),
      ackPacket_(EPacketHandlingType::Ack)
  {
    regularPacket_.Reset();
    ackPacket_.Reset();
  }

  Packet& GetPacketByHandlingType(const EPacketHandlingType InHandlingType)
  {
    switch (InHandlingType)
    {
    case EPacketHandlingType::None:
      return regularPacket_;
    case EPacketHandlingType::Ack:
      return ackPacket_;
    default: ;
    }

    throw std::runtime_error("Invalid EPacketHandlingType was used");
  }

  void PushAckPacketIfContainingData()
  {
    if (ackPacket_.IsEmpty())
    {
      return;
    }
        
    ackPacketsNotReturned_.insert({ ackPacket_.GetIdentifier(), ackPacket_ });
    ackPacket_.Reset();

    std::cout << "Added ack packet " << ackPacket_.GetIdentifier() << "\n";
  }
    
  const std::map<int32_t, Packet>& GetPacketsNotReturned() const { return ackPacketsNotReturned_; }
  void RemoveReturnedPacket(const int32_t InIdentifier)
  {
    ackPacketsNotReturned_.erase(InIdentifier);
  }
    
private:
  Packet regularPacket_;
  Packet ackPacket_;
    
  std::map<int32_t, Packet> ackPacketsNotReturned_;
};
}