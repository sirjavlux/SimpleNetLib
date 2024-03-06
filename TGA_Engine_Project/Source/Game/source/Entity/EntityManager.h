#pragma once

#include <map>

#include "Entities\Entity.hpp"
#include "SimpleNetLib.h"
#include "../PacketComponents/RequestSpawnEntityComponent.hpp"
#include "../PacketComponents/SpawnEntityComponent.hpp"
#include "Utility/HashUtility.hpp"

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
  void RequestSpawnEntity(const NetTag& InEntityTypeTag) const;

  // Client sided function
  void RequestDestroyEntity(uint16_t InIdentifier);
  
  // This is a server sided function deciding various data like the entity id
  Entity* SpawnEntityServer(const NetTag& InEntityTypeTag, const Tga::Vector2f& InPosition = { 0.f, 0.f }, const Tga::Vector2f& InDir = { 0.f, 0.f });
  Entity* SpawnEntityLocal(const NetTag& InEntityTypeTag, const Tga::Vector2f& InPosition = { 0.f, 0.f }, const Tga::Vector2f& InDir = { 0.f, 0.f });

  static bool IsServer();

  // Client sided
  void OnEntitySpawnReceived(const sockaddr_storage& InAddress, const Net::PacketComponent& InComponent);
  void OnEntityDespawnReceived(const sockaddr_storage& InAddress, const Net::PacketComponent& InComponent);
  void OnKickedFromServerReceived(const sockaddr_storage& InAddress, const Net::PacketComponent& InComponent); // TODO: Needs more player feedback
  
  // Server sided
  void OnEntitySpawnRequestReceived(const sockaddr_storage& InAddress, const Net::PacketComponent& InComponent);
  void OnEntityDespawnRequestReceived(const sockaddr_storage& InAddress, const Net::PacketComponent& InComponent);
  void OnConnectionReceived(const sockaddr_storage& InAddress, const Net::PacketComponent& InComponent);
  void OnInputReceived(const sockaddr_storage& InAddress, const Net::PacketComponent& InComponent);
  void OnControllerPositionUpdateReceived(const sockaddr_storage& InAddress, const Net::PacketComponent& InComponent);
  void OnPositionUpdateReceived(const sockaddr_storage& InAddress, const Net::PacketComponent& InComponent);
  void OnSetEntityPossessedReceived(const sockaddr_storage& InAddress, const Net::PacketComponent& InComponent);
  void OnReturnAckReceived(const sockaddr_storage& InAddress, const Net::PacketComponent& InComponent);
  void OnClientDisconnect(const sockaddr_storage& InAddress, uint8_t InDisconnectType);

  bool IsUsernameTaken(const std::string& InUsername) const;
  
  template<typename EntityType>
  void RegisterEntityTemplate(NetTag InTag);

  void SetPossessedEntityByNetTarget(const sockaddr_storage& InAddress, uint16_t InIdentifier);
  
  Entity* GetPossessedEntity() { return possessedEntity_; }

  Entity* GetEntityById(const uint16_t InIdentifier)
  {
    return entities_.find(InIdentifier) != entities_.end() ? entities_.at(InIdentifier).get() : nullptr;
  }

  // Server sided
  void MarkEntityForDestruction(const uint16_t InIdentifier)
  {
    if (Net::PacketManager::Get()->IsServer())
    {
      entitiesToDestroy_.push_back(InIdentifier);
    }
  }
  
private:
  // This is a server sided function
  void DestroyEntityServer(uint16_t InIdentifier);
  
  Entity* AddEntity(uint64_t InEntityTypeHash, uint16_t InIdentifier, bool InIsLocallySpawned = false);
  bool RemoveEntity(uint16_t InIdentifier, bool InIsLocallySpawned = false);
  
  void UpdateEntityPossession();
  
  template<typename EntityType>
  bool IsEntityTypeValid() const;
  
  std::shared_ptr<Entity> CreateNewEntityFromTemplate(const uint64_t& InTypeHash);
  
  static uint16_t GenerateEntityIdentifier();

  void RegisterPacketComponents();
  void SubscribeToPacketComponents();

  std::vector<uint16_t> entitiesToDestroy_;
  
  std::map<uint16_t, std::shared_ptr<Entity>> entities_;
  std::map<uint16_t, std::shared_ptr<Entity>> entitiesLocal_;

  bool bPossession_ = false;
  std::string possessionUsername_;
  int16_t entityToUpdatePossess_ = -1;
  
  using EntityCreator = std::function<std::shared_ptr<Entity>()>;
  std::map<uint64_t, EntityCreator> entityFactoryMap_;

  std::map<sockaddr_storage, uint16_t> entitiesPossessed_;
  Entity* possessedEntity_ = nullptr;

  std::chrono::steady_clock::time_point lastUpdateTime_;
  double updateLag_ = 0.0;
  
  static EntityManager* instance_;
};

template <typename EntityType>
void EntityManager::RegisterEntityTemplate(const NetTag InTag)
{
  if (IsEntityTypeValid<EntityType>() && entityFactoryMap_.find(InTag.GetHash()) == entityFactoryMap_.end())
  {
    uint64_t hash = InTag.GetHash();
    entityFactoryMap_.insert({hash, [hash]()
    {
      std::shared_ptr<Entity> newEntity = std::make_shared<EntityType>();
      newEntity->typeTagHash_ = hash;
      return newEntity;
    }
    });
  }
}

template <typename EntityType>
bool EntityManager::IsEntityTypeValid() const
{
  const bool isDerived = std::is_base_of_v<Entity, EntityType>;
  return isDerived;
}
