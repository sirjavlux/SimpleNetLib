#pragma once

#include <random>

#include "Entities\Entity.h"
#include "EntityComponents\EntityComponent.h"
#include "Packet/PacketManager.h"
#include "Utility/HashUtility.hpp"
#include "../PacketComponents/SetEntityPossessedComponent.hpp"
#include "../Entity/EntityComponents/ColliderComponent.h"

class PlayerShipEntity;
class EntityManager
{
public:
  EntityManager();
  ~EntityManager();
    
  static EntityManager* Initialize();
  static EntityManager* Get();
  static void End();

  void UpdateEntities(float InDeltaTime);
  
  // Client sided function
  void RequestSpawnEntity(const NetTag& InEntityTypeTag) const;

  // Client sided function
  void RequestDestroyEntity(uint16_t InIdentifier);
  
  // This is a server sided function deciding various data like the entity id
  Entity* SpawnEntityServer(const NetTag& InEntityTypeTag, const Tga::Vector2f& InPosition = { 0.f, 0.f }, const Tga::Vector2f& InDir = { 0.f, 0.f });
  Entity* SpawnEntityLocal(const NetTag& InEntityTypeTag, const Tga::Vector2f& InPosition = { 0.f, 0.f }, const Tga::Vector2f& InDir = { 0.f, 0.f });

  static bool IsServer();

  static Tga::Vector2f GenerateRandomSpawnPos();
  static void RespawnPlayerAtRandomPos(PlayerShipEntity* InPlayerEntity);
  
  // Client sided
  void OnEntitySpawnReceived(const sockaddr_storage& InAddress, const Net::PacketComponent& InComponent);
  void OnEntityDespawnReceived(const sockaddr_storage& InAddress, const Net::PacketComponent& InComponent);
  void OnKickedFromServerReceived(const sockaddr_storage& InAddress, const Net::PacketComponent& InComponent); // TODO: Needs more player feedback
  void OnReadReplication(const sockaddr_storage& InAddress, const Net::PacketComponent& InComponent);
  void OnAddEntityComponentReceived(const sockaddr_storage& InAddress, const Net::PacketComponent& InComponent);
  void OnSetEntityPossessedReceived(const sockaddr_storage& InAddress, const Net::PacketComponent& InComponent);
  
  // Server sided
  void OnEntitySpawnRequestReceived(const sockaddr_storage& InAddress, const Net::PacketComponent& InComponent);
  void OnEntityDespawnRequestReceived(const sockaddr_storage& InAddress, const Net::PacketComponent& InComponent);
  void OnConnectionReceived(const sockaddr_storage& InAddress, const Net::PacketComponent& InComponent);
  void OnInputReceived(const sockaddr_storage& InAddress, const Net::PacketComponent& InComponent);
  void OnControllerPositionUpdateReceived(const sockaddr_storage& InAddress, const Net::PacketComponent& InComponent);
  void OnPositionUpdateReceived(const sockaddr_storage& InAddress, const Net::PacketComponent& InComponent);
  void OnReturnAckReceived(const sockaddr_storage& InAddress, const Net::PacketComponent& InComponent);
  void OnEntitySpawnHasBeenReceived(const sockaddr_storage& InAddress, const Net::PacketComponent& InComponent);
  void OnEntityComponentAddHasBeenReceived(const sockaddr_storage& InAddress, const Net::PacketComponent& InComponent);
  void OnClientDisconnect(const sockaddr_storage& InAddress, uint8_t InDisconnectType);

  bool IsUsernameTaken(const std::string& InUsername) const;
  
  template<typename EntityType>
  void RegisterEntityTemplate(NetTag InTag);

  template<typename EntityComponentType>
  void RegisterEntityComponentTemplate(NetTag InTag);

  void SetPossessedEntityByNetTarget(const sockaddr_in& InAddress, uint16_t InIdentifier);
  const std::unordered_map<sockaddr_in, uint16_t>& GetEntityPossessionMap() const { return netTargetEntityMap_; }
  uint16_t GetEntityPossessedByNetTarget(const sockaddr_in& InAddress);
  sockaddr_in GetNetTargetPossessingEntity(uint16_t InIdentifier);
  
  // Client sided function
  Entity* GetPossessedEntity() { return possessedEntity_; }

  Entity* GetEntityById(const uint16_t InIdentifier)
  {
    return entities_.find(InIdentifier) != entities_.end() ? entities_.at(InIdentifier).get() : nullptr;
  }

  Entity* GetLocalEntityById(const uint16_t InIdentifier)
  {
    return entitiesLocal_.find(InIdentifier) != entitiesLocal_.end() ? entitiesLocal_.at(InIdentifier).get() : nullptr;
  }

  // Server sided
  void MarkEntityForDestruction(const uint16_t InIdentifier)
  {
    if (Net::PacketManager::Get()->IsServer())
    {
      entitiesToDestroy_.push_back(InIdentifier);
    }
  }

  std::default_random_engine& GetRandomEngine() {return random_Engine_; }

  // This is a server sided function
  void DestroyEntityServer(uint16_t InIdentifier);

  template<typename EntityComponentType>
  EntityComponentType* AddComponentToEntityOfType(uint16_t InEntityIdentifier, uint64_t InComponentTypeHash, uint16_t InComponentIdentifier = 0);
  EntityComponent* AddComponentToEntity(uint16_t InEntityIdentifier, uint64_t InComponentTypeHash, uint16_t InComponentIdentifier = 0);
  
private:

  void InitializeEntity(Entity* entity);
  
  void RegisterEntityComponents();
  
  Entity* AddEntity(uint64_t InEntityTypeHash, uint16_t InIdentifier, bool InIsLocallySpawned = false);
  bool RemoveEntity(uint16_t InIdentifier, bool InIsLocallySpawned = false);
  
  template<typename EntityType>
  bool IsEntityTypeValid() const;

  template <typename EntityComponentType>
  bool IsEntityComponentTypeValid() const;
  
  std::shared_ptr<Entity> CreateNewEntityFromTemplate(const uint64_t& InTypeHash);
  
  std::shared_ptr<EntityComponent> CreateNewEntityComponentFromTemplate(const uint64_t& InTypeHash);
  
  static uint16_t GenerateEntityIdentifier();
  static uint16_t GenerateEntityComponentIdentifier();

  void RegisterPacketComponents();
  void SubscribeToPacketComponents();

  std::vector<uint16_t> entitiesToDestroy_;
  
  std::unordered_map<uint16_t, std::shared_ptr<Entity>> entities_;
  std::unordered_map<uint16_t, std::shared_ptr<Entity>> entitiesLocal_;
  
  using EntityCreator = std::function<std::shared_ptr<Entity>()>;
  std::unordered_map<uint64_t, EntityCreator> entityFactoryMap_;

  using EntityComponentCreator = std::function<std::shared_ptr<EntityComponent>()>;
  std::unordered_map<uint64_t, EntityComponentCreator> entityComponentFactoryMap_;

  std::unordered_map<uint16_t, sockaddr_in> entityNetTargetMap_;
  std::unordered_map<sockaddr_in, uint16_t> netTargetEntityMap_;
  Entity* possessedEntity_ = nullptr;

  std::chrono::steady_clock::time_point lastUpdateTime_;
  double updateLag_ = 0.0;

  std::default_random_engine random_Engine_;

  static EntityManager* instance_;
};

template <typename EntityComponentType>
EntityComponentType* EntityManager::AddComponentToEntityOfType(const uint16_t InEntityIdentifier, const uint64_t InComponentTypeHash, const uint16_t InComponentIdentifier)
{
  EntityComponentType* result = nullptr;
  if (!IsEntityComponentTypeValid<EntityComponentType>())
  {
    return result;
  }
  
  EntityComponent* entityComponent = AddComponentToEntity(InEntityIdentifier, InComponentTypeHash, InComponentIdentifier); 
  result = static_cast<EntityComponentType*>(entityComponent);
  
  return result;
}

template <typename EntityType>
void EntityManager::RegisterEntityTemplate(const NetTag InTag)
{
  const uint64_t hash = InTag.GetHash();
  if (IsEntityTypeValid<EntityType>() && entityFactoryMap_.find(hash) == entityFactoryMap_.end())
  {
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

template <typename EntityComponentType>
void EntityManager::RegisterEntityComponentTemplate(const NetTag InTag)
{
  const uint64_t hash = InTag.GetHash();
  if (entityComponentFactoryMap_.find(hash) == entityComponentFactoryMap_.end())
  {
    entityComponentFactoryMap_.insert({hash, [hash]()
    {
      std::shared_ptr<EntityComponent> newEntityComponent = std::make_shared<EntityComponentType>();
      newEntityComponent->typeTagHash_ = hash;
      return newEntityComponent;
    }
    });
  }
}

template <typename EntityComponentType>
bool EntityManager::IsEntityComponentTypeValid() const
{
  const bool isDerived = std::is_base_of_v<EntityComponent, EntityComponentType>;
  return isDerived;
}