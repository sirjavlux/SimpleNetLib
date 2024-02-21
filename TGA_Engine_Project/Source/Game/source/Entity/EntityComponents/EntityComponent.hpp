#pragma once

class Entity;

class EntityComponent
{
public:
  void SetOwner(Entity& InOwner)
  {
    owner_ = &InOwner;
  }
  
  virtual ~EntityComponent() = default;

  virtual void Init() = 0;
  
  virtual void Update(float InDeltaTime) = 0;
  
protected:
  Entity* owner_ = nullptr;
};
