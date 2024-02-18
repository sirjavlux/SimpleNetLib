#pragma once

#include <map>

#include "Entities\Entity.hpp"
#include "SimpleNetLib.h"

class EntityManager
{
public:
  EntityManager();
  ~EntityManager();
    
  static EntityManager* Initialize();
  static EntityManager* Get();
  static void End();

  void UpdateEntities(float InDeltaTime);
  void RenderEntities();
  
  // Client sided function
  template<typename EntityType>
  Entity* RequestSpawnEntity(); // TODO:

  // Client sided function
  void RequestDestroyEntity(uint16_t InIdentifier); // TODO:
  
  // This is a server sided function deciding various data like the entity id
  template<typename EntityType>
  Entity* SpawnEntity(); // TODO:

  // This is a server sided function
  void DestroyEntity(uint16_t InIdentifier); // TODO:

  static bool IsServer();

  // Client sided
  void OnEntitySpawnReceived(const sockaddr_storage& InTarget, const Net::PacketComponent& InComponent); // TODO:
  void OnEntityDespawnReceived(const sockaddr_storage& InTarget, const Net::PacketComponent& InComponent); // TODO:
  
  // Server sided
  void OnEntitySpawnRequestReceived(const sockaddr_storage& InTarget, const Net::PacketComponent& InComponent); // TODO:
  void OnEntityDespawnRequestReceived(const sockaddr_storage& InTarget, const Net::PacketComponent& InComponent); // TODO:
  
private:
  template<typename EntityType>
  bool IsEntityTypeValid() const;
  
  template<typename EntityType>
  Entity* AddEntity(uint16_t InIdentifier);

  static uint16_t GenerateEntityIdentifier();

  void RegisterPacketComponents(); // TODO:
  
  std::map<uint16_t, std::shared_ptr<Entity>> entities_;

  static EntityManager* instance_;
};

template <typename EntityType>
Entity* EntityManager::RequestSpawnEntity()
{
  if (IsEntityTypeValid<EntityType>())
  {
    return nullptr;
  }

  
}

template <typename EntityType>
Entity* EntityManager::SpawnEntity()
{
  if (!IsServer() || IsEntityTypeValid<EntityType>())
  {
    return nullptr;
  }

  
  
  return AddEntity<EntityType>(GenerateEntityIdentifier());
}

template <typename EntityType>
bool EntityManager::IsEntityTypeValid() const
{
  bool isDerived = std::is_base_of_v<Entity, EntityType>;
  static_assert(isDerived, "EntityType must be derived from Entity!");
  return isDerived;
}

template <typename EntityType>
Entity* EntityManager::AddEntity(uint16_t InIdentifier)
{
  std::shared_ptr<EntityType> newEntity = std::make_shared<EntityType>();
  newEntity->Init();
  
  entities_.insert({ InIdentifier, newEntity });
  
  return newEntity.get();
}
