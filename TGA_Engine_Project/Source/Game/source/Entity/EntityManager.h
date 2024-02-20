#pragma once

#include <map>

#include "Entities\Entity.hpp"
#include "SimpleNetLib.h"
#include "../PacketComponents/RequestSpawnEntityComponent.hpp"
#include "../PacketComponents/SpawnEntityComponent.hpp"

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
  void RequestSpawnEntity(const NetTag& InEntityTypeTag) const;

  // Client sided function
  void RequestDestroyEntity(uint16_t InIdentifier);
  
  // This is a server sided function deciding various data like the entity id
  template<typename EntityType>
  Entity* SpawnEntityServer();

  // This is a server sided function
  void DestroyEntityServer(uint16_t InIdentifier);

  static bool IsServer();

  // Client sided
  void OnEntitySpawnReceived(const sockaddr_storage& InTarget, const Net::PacketComponent& InComponent);
  void OnEntityDespawnReceived(const sockaddr_storage& InTarget, const Net::PacketComponent& InComponent);
  
  // Server sided
  void OnEntitySpawnRequestReceived(const sockaddr_storage& InTarget, const Net::PacketComponent& InComponent);
  void OnEntityDespawnRequestReceived(const sockaddr_storage& InTarget, const Net::PacketComponent& InComponent);
  void OnConnectionReceived(const sockaddr_storage& InTarget, const Net::PacketComponent& InComponent); // TODO:
  
  template<typename EntityType>
  void RegisterEntityTemplate(NetTag InTag); // TODO: Needs testing
  
private:
  template<typename EntityType>
  bool IsEntityTypeValid() const;
  
  template<typename EntityType>
  Entity* AddEntity(uint16_t InIdentifier);
  Entity* AddEntity(uint64_t InEntityTypeHash, uint16_t InIdentifier);
  bool RemoveEntity(uint16_t InIdentifier);
  
  std::shared_ptr<Entity> CreateNewEntityFromTemplate(const uint64_t& InTypeHash);
  
  static uint16_t GenerateEntityIdentifier();

  void RegisterPacketComponents();
  
  std::map<uint16_t, std::shared_ptr<Entity>> entities_;

  using EntityCreator = std::function<std::shared_ptr<Entity>()>;
  std::map<uint64_t, EntityCreator> entityFactoryMap_;
  
  static EntityManager* instance_;
};

template <typename EntityType>
void EntityManager::RequestSpawnEntity(const NetTag& InEntityTypeTag) const
{
  if (IsEntityTypeValid<EntityType>())
  {
    return;
  }

  RequestSpawnEntityComponent requestSpawnEntityComponent;
  requestSpawnEntityComponent.entityTypeHash = InEntityTypeTag.GetHash();
  Net::PacketManager::Get()->SendPacketComponentToParent<RequestSpawnEntityComponent>(requestSpawnEntityComponent);
}

template <typename EntityType>
Entity* EntityManager::SpawnEntityServer()
{
  if (!IsServer() || IsEntityTypeValid<EntityType>())
  {
    return nullptr;
  }

  Entity* newEntity = AddEntity<EntityType>(GenerateEntityIdentifier());
  
  SpawnEntityComponent spawnEntityComponent;
  spawnEntityComponent.entityId = newEntity->GetId();
  spawnEntityComponent.entityTypeHash = newEntity->GetTypeTag().GetHash();
  Net::PacketManager::Get()->SendPacketComponentMulticast<SpawnEntityComponent>(spawnEntityComponent);
  
  return newEntity;
}

template <typename EntityType>
void EntityManager::RegisterEntityTemplate(const NetTag InTag)
{
  if (IsEntityTypeValid<EntityType>() && entityFactoryMap_.find(InTag.GetHash()) == entityFactoryMap_.end())
  {
    entityFactoryMap_.insert({InTag, [InTag]()
    {
      std::shared_ptr<Entity> newEntity = std::make_shared<EntityType>();
      newEntity->typeTag_ = InTag;
      return newEntity;
    }
    });
  }
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
  newEntity->id_ = InIdentifier;
  newEntity->Init();
  
  entities_.insert({ InIdentifier, newEntity });
  
  return newEntity.get();
}
