#include "stdafx.h"
#include "EntityManager.h"

#include <ffmpeg-2.0/libavutil/mathematics.h>

#include "../Definitions.hpp"
#include "../GameWorld.h"
#include "../Combat/BulletManager.h"
#include "../PacketComponents/DeSpawnEntityComponent.hpp"
#include "../PacketComponents/InputComponent.hpp"
#include "../PacketComponents/SpawnEntityComponent.hpp"
#include "../PacketComponents/RequestDeSpawnEntityComponent.hpp"
#include "../PacketComponents/RequestSpawnEntityComponent.hpp"
#include "../PacketComponents/SetEntityPossessedComponent.hpp"
#include "../PacketComponents/UpdateEntityPositionComponent.hpp"
#include "../PacketComponents/UpdateEntityControllerPositionComponent.hpp"
#include "Entities/PlayerShipEntity.h"
#include "EntityComponents/ControllerComponent.h"
#include "Events/EventSystem.h"
#include "Packet/CorePacketComponents/ReturnAckComponent.hpp"
#include "Packet/CorePacketComponents/ServerConnectPacketComponent.hpp"
#include "../Locator/Locator.h"
#include "Packet/CorePacketComponents/KickedFromServerPacketComponent.hpp"
#include "tge/drawers/DebugDrawer.h"

EntityManager* EntityManager::instance_ = nullptr;

EntityManager::EntityManager()
{
  
}

EntityManager::~EntityManager()
{
  
}

EntityManager* EntityManager::Initialize()
{
  using namespace std::chrono;
  
  if (instance_ == nullptr)
  {
    instance_ = new EntityManager();
    instance_->lastUpdateTime_ = steady_clock::now();
    instance_->RegisterPacketComponents();
    instance_->SubscribeToPacketComponents();
  }
  return instance_;
}

EntityManager* EntityManager::Get()
{
  return instance_;
}

void EntityManager::End()
{
  if (instance_ != nullptr)
  {
    Net::PacketComponentDelegator* componentDelegator = Net::SimpleNetLibCore::Get()->GetPacketComponentDelegator();
    componentDelegator->UnSubscribeFromPacketComponentDelegate<DeSpawnEntityComponent, EntityManager>(&EntityManager::OnEntityDespawnReceived, instance_);
    componentDelegator->UnSubscribeFromPacketComponentDelegate<SpawnEntityComponent, EntityManager>(&EntityManager::OnEntitySpawnReceived, instance_);
    componentDelegator->UnSubscribeFromPacketComponentDelegate<RequestDeSpawnEntityComponent, EntityManager>(&EntityManager::OnEntityDespawnRequestReceived, instance_);
    componentDelegator->UnSubscribeFromPacketComponentDelegate<RequestSpawnEntityComponent, EntityManager>(&EntityManager::OnEntitySpawnRequestReceived, instance_);
    componentDelegator->UnSubscribeFromPacketComponentDelegate<SetEntityPossessedComponent, EntityManager>(&EntityManager::OnSetEntityPossessedReceived, instance_);
    componentDelegator->UnSubscribeFromPacketComponentDelegate<InputComponent, EntityManager>(&EntityManager::OnInputReceived, instance_);
    componentDelegator->UnSubscribeFromPacketComponentDelegate<UpdateEntityControllerPositionComponent, EntityManager>(&EntityManager::OnControllerPositionUpdateReceived, instance_);
    componentDelegator->UnSubscribeFromPacketComponentDelegate<UpdateEntityPositionComponent, EntityManager>(&EntityManager::OnPositionUpdateReceived, instance_);
    componentDelegator->UnSubscribeFromPacketComponentDelegate<ServerConnectPacketComponent, EntityManager>(&EntityManager::OnConnectionReceived, instance_);
    componentDelegator->UnSubscribeFromPacketComponentDelegate<ReturnAckComponent, EntityManager>(&EntityManager::OnReturnAckReceived, instance_);
    componentDelegator->UnSubscribeFromPacketComponentDelegate<KickedFromServerPacketComponent, EntityManager>(&EntityManager::OnKickedFromServerReceived, instance_);
    componentDelegator->UnSubscribeFromPacketComponentDelegate<DataReplicationPacketComponent, EntityManager>(&EntityManager::OnReadReplication, instance_);

    Net::EventSystem::Get()->onClientDisconnectEvent.RemoveDynamic<EntityManager>(instance_, &EntityManager::OnClientDisconnect);
    
    delete instance_;
    instance_ = nullptr;
  }
}

void EntityManager::UpdateEntities(const float InDeltaTime)
{
  using namespace std::chrono;

  const steady_clock::time_point currentTime = steady_clock::now();
  const duration<double> deltaTime = duration_cast<duration<double>>(currentTime - lastUpdateTime_);
  lastUpdateTime_ = currentTime;

  updateLag_ += deltaTime.count();

  if (updateLag_ >= FIXED_UPDATE_DELTA_TIME)
  {
    // Check possession
    UpdateEntityPossession();

    // FixedUpdate entities
    for (auto& entityData : entities_)
    {
      entityData.second->FixedUpdate();
      entityData.second->FixedUpdateComponents();
      entityData.second->NativeFixedUpdate();
    }
    for (auto& entityData : entitiesLocal_)
    {
      entityData.second->FixedUpdate();
      entityData.second->FixedUpdateComponents();
      entityData.second->NativeFixedUpdate();
    }
    
    updateLag_ -= FIXED_UPDATE_DELTA_TIME;
  }
  
  // Update entities
  for (auto& entityData : entities_)
  {
    entityData.second->Update(InDeltaTime);
    entityData.second->UpdateComponents(InDeltaTime);
    entityData.second->UpdateSmoothMovement(InDeltaTime);
  }
  for (auto& entityData : entitiesLocal_)
  {
    entityData.second->Update(InDeltaTime);
    entityData.second->UpdateComponents(InDeltaTime);
    entityData.second->UpdateSmoothMovement(InDeltaTime);
  }

  // Destroy entities marked for destruction
  for (const uint16_t id : entitiesToDestroy_)
  {
    DestroyEntityServer(id);
  }
  entitiesToDestroy_.clear();
}

void EntityManager::RenderEntities()
{
  for (auto& entityData : entities_)
  {
    entityData.second->UpdateRender();
  }
  for (auto& entityData : entitiesLocal_)
  {
    entityData.second->UpdateRender();
  }
}

void EntityManager::RequestSpawnEntity(const NetTag& InEntityTypeTag) const
{
  if (entityFactoryMap_.find(InEntityTypeTag.GetHash()) == entityFactoryMap_.end())
  {
    return;
  }

  RequestSpawnEntityComponent requestSpawnEntityComponent;
  requestSpawnEntityComponent.entityTypeHash = InEntityTypeTag.GetHash();
  Net::PacketManager::Get()->SendPacketComponentToParent<RequestSpawnEntityComponent>(requestSpawnEntityComponent);
}

void EntityManager::RequestDestroyEntity(const uint16_t InIdentifier)
{
  if (entities_.find(InIdentifier) == entities_.end())
  {
    return;
  }

  RequestDeSpawnEntityComponent deSpawnRequestEntityComponent;
  deSpawnRequestEntityComponent.entityId = InIdentifier;
  Net::PacketManager::Get()->SendPacketComponentToParent(deSpawnRequestEntityComponent);
}

Entity* EntityManager::SpawnEntityServer(const NetTag& InEntityTypeTag, const Tga::Vector2f& InPosition, const Tga::Vector2f& InDir)
{
  if (!IsServer() || entityFactoryMap_.find(InEntityTypeTag.GetHash()) == entityFactoryMap_.end())
  {
    return nullptr;
  }
  
  Entity* newEntity = AddEntity(InEntityTypeTag.GetHash(), GenerateEntityIdentifier());
  newEntity->SetPosition({InPosition.x, InPosition.y}, true);

  const float rotation = std::atan2(InDir.y, InDir.x);
  newEntity->SetDirection({InDir.x, InDir.y});
  
  SpawnEntityComponent spawnEntityComponent;
  spawnEntityComponent.entityId = newEntity->GetId();
  spawnEntityComponent.entityTypeHash = newEntity->GetTypeTagHash();
  spawnEntityComponent.xPos = InPosition.x;
  spawnEntityComponent.yPos = InPosition.y;
  spawnEntityComponent.rotation = rotation;
  Net::PacketManager::Get()->SendPacketComponentMulticast<SpawnEntityComponent>(spawnEntityComponent);

  newEntity->UpdateReplication();
  
  return newEntity;
}

Entity* EntityManager::SpawnEntityLocal(const NetTag& InEntityTypeTag, const Tga::Vector2f& InPosition, const Tga::Vector2f& InDir)
{
  if (IsServer() || entityFactoryMap_.find(InEntityTypeTag.GetHash()) == entityFactoryMap_.end())
  {
    return nullptr;
  }

  Entity* newEntity = AddEntity(InEntityTypeTag.GetHash(), GenerateEntityIdentifier(), true);
  newEntity->SetPosition({InPosition.x, InPosition.y}, true);

  newEntity->SetDirection(InDir);
  
  return newEntity;
}

void EntityManager::DestroyEntityServer(const uint16_t InIdentifier)
{
  if (!IsServer() || !RemoveEntity(InIdentifier))
  {
    return;
  }
  
  DeSpawnEntityComponent deSpawnEntityComponent;
  deSpawnEntityComponent.entityId = InIdentifier;
  Net::PacketManager::Get()->SendPacketComponentMulticast<DeSpawnEntityComponent>(deSpawnEntityComponent);
}

bool EntityManager::IsServer()
{
  return Net::PacketManager::Get()->GetManagerType() == ENetworkHandleType::Server;
}

void EntityManager::OnEntitySpawnReceived(const sockaddr_storage& InAddress, const Net::PacketComponent& InComponent)
{
  const SpawnEntityComponent* component = reinterpret_cast<const SpawnEntityComponent*>(&InComponent);
  Entity* entitySpawned = AddEntity(component->entityTypeHash, component->entityId);
  if (entitySpawned == nullptr)
  {
    return;
  }
  
  entitySpawned->SetPosition({ component->xPos, component->yPos }, true);

  const float xDir = std::cos(component->rotation);
  const float yDir = std::sin(component->rotation);
  entitySpawned->SetDirection({ xDir, yDir });
  
  //std::cout << "Spawn entity received! " << entitySpawned->GetTypeTagHash() << " : " <<  component->entityId << "\n";
}

void EntityManager::OnEntityDespawnReceived(const sockaddr_storage& InAddress, const Net::PacketComponent& InComponent)
{
  const DeSpawnEntityComponent* component = reinterpret_cast<const DeSpawnEntityComponent*>(&InComponent);
  RemoveEntity(component->entityId);
}

void EntityManager::OnKickedFromServerReceived(const sockaddr_storage& InAddress, const Net::PacketComponent& InComponent)
{
  const KickedFromServerPacketComponent* component = reinterpret_cast<const KickedFromServerPacketComponent*>(&InComponent);

  // TODO: Tell the player, username was already in use
}

void EntityManager::OnReadReplication(const sockaddr_storage& InAddress, const Net::PacketComponent& InComponent)
{
  const DataReplicationPacketComponent* component = reinterpret_cast<const DataReplicationPacketComponent*>(&InComponent);

  if (entities_.find(component->identifierDataFirst) != entities_.end())
  {
    Entity* entity = entities_.at(component->identifierDataFirst).get();
    const DataReplicationPacketComponent componentCpy = *component;
    componentCpy.variableDataObject.Begin();
    entity->OnReadReplication(componentCpy);
  }
}

void EntityManager::OnEntitySpawnRequestReceived(const sockaddr_storage& InAddress, const Net::PacketComponent& InComponent)
{
  const RequestSpawnEntityComponent* component = reinterpret_cast<const RequestSpawnEntityComponent*>(&InComponent);
  // TODO: Nothing at the moment, needs security if implemented
}

void EntityManager::OnEntityDespawnRequestReceived(const sockaddr_storage& InAddress, const Net::PacketComponent& InComponent)
{
  const RequestDeSpawnEntityComponent* component = reinterpret_cast<const RequestDeSpawnEntityComponent*>(&InComponent);
  // TODO: Nothing at the moment, needs security if implemented
}

void EntityManager::OnConnectionReceived(const sockaddr_storage& InAddress, const Net::PacketComponent& InComponent)
{
  ServerConnectPacketComponent component = *reinterpret_cast<const ServerConnectPacketComponent*>(&InComponent);

  // Extract data from in packet component
  component.variableDataObject.Begin();
  char usernameBuffer[USERNAME_MAX_LENGTH];
  
  operator<<<char, CONNECTION_DATA_SIZE, USERNAME_MAX_LENGTH>(*usernameBuffer, component.variableDataObject);
  const std::string username = usernameBuffer;

  // Kick player if username is taken
  if (IsUsernameTaken(username))
  {
    Net::SimpleNetLibCore::Get()->GetNetHandler()->KickNetTarget(InAddress, static_cast<uint8_t>(EDisconnectType::UsernameTaken));
    return;
  }
  
  // Spawn existing entities // TODO: Might need to update this to not be as intense, sending data pieces culled by range
  for (const auto& entity : entities_)
  {
    SpawnEntityComponent spawnEntityComponent;
    spawnEntityComponent.entityId = entity.second->GetId();
    spawnEntityComponent.entityTypeHash = entity.second->GetTypeTagHash();
    spawnEntityComponent.xPos = entity.second->GetPosition().x;
    spawnEntityComponent.yPos = entity.second->GetPosition().y;
    Net::PacketManager::Get()->SendPacketComponent<SpawnEntityComponent>(spawnEntityComponent, InAddress);
  }
  
  // Spawn player controllable entity
  const NetTag playerTypeTag = NetTag("player.ship");
  PlayerShipEntity* entitySpawned = dynamic_cast<PlayerShipEntity*>(SpawnEntityServer(playerTypeTag));
  entitySpawned->GetFirstComponent<ControllerComponent>().lock()->SetPossessedBy(InAddress);
  entitySpawned->SetUsername(username);
  
  SetEntityPossessedComponent setEntityPossessed;
  setEntityPossessed.entityIdentifier = entitySpawned->GetId();
  setEntityPossessed.bShouldPossess = true;
  std::memcpy(setEntityPossessed.usernameBuffer, usernameBuffer, USERNAME_MAX_LENGTH);
  Net::PacketManager::Get()->SendPacketComponent<SetEntityPossessedComponent>(setEntityPossessed, InAddress);
}

void EntityManager::OnInputReceived(const sockaddr_storage& InAddress, const Net::PacketComponent& InComponent)
{
  const InputComponent* component = reinterpret_cast<const InputComponent*>(&InComponent);
  if (entities_.find(component->entityIdentifier) != entities_.end())
  {
    Entity* entity = entities_.at(component->entityIdentifier).get();
    ControllerComponent* controllerComponent = entity->GetFirstComponent<ControllerComponent>().lock().get();

    // Check if target entity is possessed by the client
    if (controllerComponent && controllerComponent->IsPossessedBy(InAddress))
    {
      InputUpdateEntry entry = {};
      entry.sequenceNr = component->sequenceNr;

      // Handle AWSD Input
      const EKeyInput input = static_cast<EKeyInput>(component->keysPressBuffer);
      if ((input & EKeyInput::W) != EKeyInput::None)
      {
        entry.yInputForce += 1.f;
      }
      if ((input & EKeyInput::S) != EKeyInput::None)
      {
        entry.yInputForce += -1.f;
      }
      if ((input & EKeyInput::A) != EKeyInput::None)
      {
        entry.xInputForce += -1.f;
      }
      if ((input & EKeyInput::D) != EKeyInput::None)
      {
        entry.xInputForce += 1.f;
      }

      entry.inputTargetDirection = component->inputTargetDirection;
      
      // Update input buffer
      controllerComponent->UpdateInputBuffer(entry);
      
      if ((input & EKeyInput::Space) != EKeyInput::None)
      {
        Locator::Get()->GetBulletManager()->TryShootBulletServer(entity->GetId());
      }
    }
  }
}

void EntityManager::OnControllerPositionUpdateReceived(const sockaddr_storage& InAddress, const Net::PacketComponent& InComponent)
{
  const UpdateEntityControllerPositionComponent* component = reinterpret_cast<const UpdateEntityControllerPositionComponent*>(&InComponent);
  if (entities_.find(component->entityIdentifier) != entities_.end())
  {
    Entity* entity = entities_.at(component->entityIdentifier).get();
    if (ControllerComponent* controllerComponent = entity->GetFirstComponent<ControllerComponent>().lock().get())
    {
      controllerComponent->UpdatePositionBuffer(component->positionUpdateEntry);
    }
  }
}

void EntityManager::OnPositionUpdateReceived(const sockaddr_storage& InAddress, const Net::PacketComponent& InComponent)
{
  const UpdateEntityPositionComponent* component = reinterpret_cast<const UpdateEntityPositionComponent*>(&InComponent);
  if (entities_.find(component->entityIdentifier) != entities_.end())
  {
    Entity* entity = entities_.at(component->entityIdentifier).get();
    if (component->bIsTeleport)
    {
      entity->SetPosition({ component->xPos, component->yPos }, component->bIsTeleport);
    } else
    {
      entity->SetTargetPosition({ component->xPos, component->yPos });
    }
    entity->SetDirection({ std::cos(component->rotation), std::sin(component->rotation) });
  }
}

void EntityManager::OnSetEntityPossessedReceived(const sockaddr_storage& InAddress, const Net::PacketComponent& InComponent)
{
  const SetEntityPossessedComponent* component = reinterpret_cast<const SetEntityPossessedComponent*>(&InComponent);
  entityPossessionBuffer_.push_back(*component);
}

void EntityManager::OnReturnAckReceived(const sockaddr_storage& InAddress, const Net::PacketComponent& InComponent)
{
  const ReturnAckComponent* component = reinterpret_cast<const ReturnAckComponent*>(&InComponent);
}

void EntityManager::OnClientDisconnect(const sockaddr_storage& InAddress, const uint8_t InDisconnectType)
{
  // TODO: Erase and handle traces of client on server
  const sockaddr_in ipv4Address = NetUtility::RetrieveIPv4AddressFromStorage(InAddress);
  if (entitiesPossessed_.find(ipv4Address) != entitiesPossessed_.end())
  {
    DestroyEntityServer(entitiesPossessed_.at(ipv4Address));
    entitiesPossessed_.erase(ipv4Address);
  }
}

bool EntityManager::IsUsernameTaken(const std::string& InUsername) const
{
  const NetTag tag = NetTag(InUsername.c_str());
  for (const auto& user : entitiesPossessed_)
  {
    const PlayerShipEntity* playerEntity = dynamic_cast<PlayerShipEntity*>(entities_.at(user.second).get());
    if (playerEntity && playerEntity->GetUsername().GetHash() == tag.GetHash())
    {
      return true;
    }
  }
  return false;
}

void EntityManager::SetPossessedEntityByNetTarget(const sockaddr_in& InAddress, uint16_t InIdentifier)
{
  if (entitiesPossessed_.find(InAddress) == entitiesPossessed_.end())
  {
    entitiesPossessed_.insert({ InAddress, InIdentifier });
  }
  entitiesPossessed_.at(InAddress) = InIdentifier;
}

void EntityManager::UpdateEntityPossession()
{
  int iter = 0;
  std::vector<int> componentsToRemove;
  for (auto& component : entityPossessionBuffer_)
  {
    if (entities_.find(component.entityIdentifier) != entities_.end())
    {
      ControllerComponent* controllerComponent = entities_.at(component.entityIdentifier)->GetFirstComponent<ControllerComponent>().lock().get();
      if (controllerComponent)
      {
        controllerComponent->SetPossessed(component.bShouldPossess);
        PlayerShipEntity* playerShip = dynamic_cast<PlayerShipEntity*>(controllerComponent->GetOwner());
        if (playerShip)
        {
          playerShip->SetUsername(component.usernameBuffer);
        }
        
        possessedEntity_ = controllerComponent->GetOwner();
        SetPossessedEntityByNetTarget(component.possessor, component.entityIdentifier);
      }
      componentsToRemove.push_back(iter);
    }
    
    ++iter;
  }

  int entitiesRemoved = 0;
  for (const int indexToRemove : componentsToRemove)
  {
    entityPossessionBuffer_.erase(entityPossessionBuffer_.begin() + indexToRemove - entitiesRemoved);
    ++entitiesRemoved;
  }
}

Entity* EntityManager::AddEntity(const uint64_t InEntityTypeHash, const uint16_t InIdentifier, const bool InIsLocallySpawned)
{
  if (entities_.find(InIdentifier) != entities_.end())
  {
    return nullptr;
  }
  
  std::shared_ptr<Entity> newEntity = CreateNewEntityFromTemplate(InEntityTypeHash);
  newEntity->id_ = InIdentifier;
  newEntity->Init();

  // Initialize Components
  for (std::shared_ptr<EntityComponent> component : newEntity->components_)
  {
    component->Init();
  }
  
  if (newEntity->renderComponent_ != nullptr)
  {
    newEntity->renderComponent_->Init();
  }

  (InIsLocallySpawned == false ? entities_ : entitiesLocal_).insert({ InIdentifier, newEntity });
  
  return newEntity.get();
}

bool EntityManager::RemoveEntity(const uint16_t InIdentifier, const bool InIsLocallySpawned)
{
  if (!InIsLocallySpawned && entities_.find(InIdentifier) != entities_.end())
  {
    entities_.erase(InIdentifier);
    return true;
  }
  if (InIsLocallySpawned && entitiesLocal_.find(InIdentifier) != entitiesLocal_.end())
  {
    entitiesLocal_.erase(InIdentifier);
    return true;
  }
  return false;
}

std::shared_ptr<Entity> EntityManager::CreateNewEntityFromTemplate(const uint64_t& InTypeHash)
{
  if (entityFactoryMap_.find(InTypeHash) != entityFactoryMap_.end())
  {
    return entityFactoryMap_.at(InTypeHash)();
  }
  return nullptr;
}

uint16_t EntityManager::GenerateEntityIdentifier()
{
  static uint16_t identifierIter = 0;
  return ++identifierIter;
}

void EntityManager::RegisterPacketComponents()
{
  const PacketComponentAssociatedData associatedDataAckComps = PacketComponentAssociatedData
  {
    false,
    FIXED_UPDATE_DELTA_TIME,
    6.f,
    EPacketHandlingType::Ack
  };
  
  Net::SimpleNetLibCore::Get()->GetPacketComponentRegistry()->RegisterPacketComponent<DeSpawnEntityComponent, EntityManager>(associatedDataAckComps, &EntityManager::OnEntityDespawnReceived, this);
  Net::SimpleNetLibCore::Get()->GetPacketComponentRegistry()->RegisterPacketComponent<SpawnEntityComponent, EntityManager>(associatedDataAckComps, &EntityManager::OnEntitySpawnReceived, this);
  Net::SimpleNetLibCore::Get()->GetPacketComponentRegistry()->RegisterPacketComponent<RequestDeSpawnEntityComponent, EntityManager>(associatedDataAckComps, &EntityManager::OnEntityDespawnRequestReceived, this);
  Net::SimpleNetLibCore::Get()->GetPacketComponentRegistry()->RegisterPacketComponent<RequestSpawnEntityComponent, EntityManager>(associatedDataAckComps, &EntityManager::OnEntitySpawnRequestReceived, this);

  Net::SimpleNetLibCore::Get()->GetPacketComponentRegistry()->RegisterPacketComponent<SetEntityPossessedComponent, EntityManager>(associatedDataAckComps, &EntityManager::OnSetEntityPossessedReceived, this);
  
  const PacketComponentAssociatedData associatedDataEveryTick = PacketComponentAssociatedData
  {
    false,
    FIXED_UPDATE_DELTA_TIME,
    1.f,
    EPacketHandlingType::None,
  };
  Net::SimpleNetLibCore::Get()->GetPacketComponentRegistry()->RegisterPacketComponent<InputComponent, EntityManager>(associatedDataEveryTick, &EntityManager::OnInputReceived, this);

  std::vector<std::pair<float, float>> packetLodFrequencies;
  packetLodFrequencies.push_back({ 1.f, 0.05f });
  packetLodFrequencies.push_back({ 2.f, 0.15f });
  packetLodFrequencies.push_back({ 4.f, 0.4f });
  const PacketComponentAssociatedData associatedEntityPositionData = PacketComponentAssociatedData
  {
    true,
    FIXED_UPDATE_DELTA_TIME,
    1.f,
    EPacketHandlingType::None,
    6.f,
    packetLodFrequencies
  };
  Net::SimpleNetLibCore::Get()->GetPacketComponentRegistry()->RegisterPacketComponent<UpdateEntityControllerPositionComponent, EntityManager>(associatedEntityPositionData, &EntityManager::OnControllerPositionUpdateReceived, this);
  Net::SimpleNetLibCore::Get()->GetPacketComponentRegistry()->RegisterPacketComponent<UpdateEntityPositionComponent, EntityManager>(associatedEntityPositionData, &EntityManager::OnPositionUpdateReceived, this);
  
  Net::EventSystem::Get()->onClientDisconnectEvent.AddDynamic<EntityManager>(this, &EntityManager::OnClientDisconnect);
}

void EntityManager::SubscribeToPacketComponents()
{
  Net::PacketComponentDelegator* componentDelegator = Net::SimpleNetLibCore::Get()->GetPacketComponentDelegator();

  componentDelegator->SubscribeToPacketComponentDelegate<ServerConnectPacketComponent, EntityManager>(&EntityManager::OnConnectionReceived, this);
  componentDelegator->SubscribeToPacketComponentDelegate<ReturnAckComponent, EntityManager>(&EntityManager::OnReturnAckReceived, this);
  componentDelegator->SubscribeToPacketComponentDelegate<KickedFromServerPacketComponent, EntityManager>(&EntityManager::OnKickedFromServerReceived, this);
  componentDelegator->SubscribeToPacketComponentDelegate<DataReplicationPacketComponent, EntityManager>(&EntityManager::OnReadReplication, this);
}