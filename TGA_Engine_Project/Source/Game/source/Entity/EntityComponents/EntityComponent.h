#pragma once

#include <cstdint>

#include "Packet/PacketManager.h"
#include "Packet/CorePacketComponents/DataReplicationPacketComponent.hpp"

#include "..\Entities\Replicable.h"

class DataReplicationPacketComponent;
class Entity;

class EntityComponent : public Replicable
{
public:
  EntityComponent();
  
  void SetOwner(Entity& InOwner) { owner_ = &InOwner; }
  const Entity* GetOwner() const { return owner_; }
  Entity* GetOwner() { return owner_; }
  
  virtual void Init() = 0;
  
  virtual void Update(float /*InDeltaTime*/) {}
  virtual void FixedUpdate() {}
  
  uint16_t GetId() const { return id_; }

  const uint64_t& GetTypeHash() const { return typeTagHash_; }

protected:
  virtual void OnDestruction() {}
  
  Entity* owner_ = nullptr;
  
  uint16_t id_;
  uint64_t typeTagHash_;
  
  friend class Entity;
  friend class EntityManager;
};
