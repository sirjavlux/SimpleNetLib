#pragma once
#include <cstdint>
#include <memory>

class Entity;

class EntityComponent
{
public:
  void SetOwner(Entity& InOwner) { owner_ = &InOwner; }
  const Entity* GetOwner() const { return owner_; }
  Entity* GetOwner() { return owner_; }
  
  virtual ~EntityComponent() = default;

  virtual void Init() = 0;
  
  virtual void Update(float InDeltaTime) {}
  virtual void FixedUpdate() {}

  uint16_t GetLocalId() const { return localId_; }
  
protected:
  Entity* owner_ = nullptr;
  
  uint16_t localId_;
  
  friend class Entity;
};
