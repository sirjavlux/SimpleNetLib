#pragma once

#include <memory>

class Entity;

class EntityComponent
{
public:
  explicit EntityComponent(const std::weak_ptr<Entity>& InOwner) : owner_(InOwner) { }
  
  virtual ~EntityComponent() = default;
  
  virtual void Update(float InDeltaTime) = 0;
  
protected:
  std::weak_ptr<Entity> owner_;
};
