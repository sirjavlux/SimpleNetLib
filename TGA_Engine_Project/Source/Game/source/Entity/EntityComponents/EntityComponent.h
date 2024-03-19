#pragma once

#include <cstdint>

#include "Packet/PacketManager.h"
#include "Packet/CorePacketComponents/DataReplicationPacketComponent.hpp"

class DataReplicationPacketComponent;
class Entity;

class EntityComponent
{
public:
  EntityComponent();
  
  void SetOwner(Entity& InOwner) { owner_ = &InOwner; }
  const Entity* GetOwner() const { return owner_; }
  Entity* GetOwner() { return owner_; }
  
  virtual void Init() = 0;
  
  virtual void Update(float /*InDeltaTime*/) {}
  virtual void FixedUpdate() {}

  // Server Sided
  virtual void OnSendReplication(DataReplicationPacketComponent& OutComponent) {}
  // Client Sided
  virtual void OnReadReplication(const DataReplicationPacketComponent& InComponent) {} 
  
  uint16_t GetId() const { return id_; }

  const uint64_t& GetTypeHash() const { return typeTagHash_; }

  bool HasBeenReceivedByClient() const { return bHasBeenReceived_; }

protected:
  virtual void OnDestruction() {}
  
  void UpdateReplication();
  void UpdateReplicationForTarget(const sockaddr_storage& InAddress);

  void UpdateReplicationPacketComponent();
  
  DataReplicationPacketComponent replicationPacketComponent_;
  DataReplicationPacketComponent oldReplicationPacketComponent_;
  
  Entity* owner_ = nullptr;
  
  uint16_t id_;
  uint64_t typeTagHash_;

  bool bHasBeenReceived_ = false;
  
  friend class Entity;
  friend class EntityManager;
};
