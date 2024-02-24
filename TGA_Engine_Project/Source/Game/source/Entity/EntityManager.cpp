#include "stdafx.h"
#include "EntityManager.h"

#include "../PacketComponents/DeSpawnEntityComponent.hpp"
#include "../PacketComponents/InputComponent.hpp"
#include "../PacketComponents/SpawnEntityComponent.hpp"
#include "../PacketComponents/RequestDeSpawnEntityComponent.hpp"
#include "../PacketComponents/RequestSpawnEntityComponent.hpp"
#include "../PacketComponents/SetEntityPossessedComponent.hpp"
#include "../PacketComponents/UpdateEntityPositionComponent.hpp"
#include "Entities/PlayerShipEntity.h"
#include "EntityComponents/ControllerComponent.h"
#include "Packet/CorePacketComponents/ReturnAckComponent.hpp"
#include "Packet/CorePacketComponents/ServerConnectPacketComponent.hpp"

EntityManager* EntityManager::instance_ = nullptr;

EntityManager::EntityManager()
{
  
}

EntityManager::~EntityManager()
{
  
}

EntityManager* EntityManager::Initialize()
{
  if (instance_ == nullptr)
  {
    instance_ = new EntityManager();
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
    delete instance_;
  }
}

void EntityManager::UpdateEntities(const float InDeltaTime)
{
  // Check possession
  if (entityToUpdatePossess_ > -1)
  {
    if (entities_.find(entityToUpdatePossess_) != entities_.end())
    {
      ControllerComponent* controllerComponent = entities_.at(entityToUpdatePossess_)->GetComponent<ControllerComponent>();
      if (controllerComponent)
      {
        controllerComponent->SetPossessed(bPossession_);
        possessedEntity_ = controllerComponent->GetOwner();
      }
      entityToUpdatePossess_ = -1; // Reset
    }
  }

  // Update entities
  for (auto& entityData : entities_)
  {
    entityData.second->Update(InDeltaTime);
    entityData.second->UpdateComponents(InDeltaTime);
  }
}

void EntityManager::RenderEntities()
{
  for (auto& entityData : entities_)
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

Entity* EntityManager::SpawnEntityServer(const NetTag& InEntityTypeTag)
{
  if (!IsServer() || entityFactoryMap_.find(InEntityTypeTag.GetHash()) == entityFactoryMap_.end())
  {
    return nullptr;
  }

  Entity* newEntity = AddEntity(InEntityTypeTag.GetHash(), GenerateEntityIdentifier());
  
  SpawnEntityComponent spawnEntityComponent;
  spawnEntityComponent.entityId = newEntity->GetId();
  spawnEntityComponent.entityTypeHash = newEntity->GetTypeTag().GetHash();
  Net::PacketManager::Get()->SendPacketComponentMulticast<SpawnEntityComponent>(spawnEntityComponent);
  
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

void EntityManager::OnEntitySpawnReceived(const sockaddr_storage& InTarget, const Net::PacketComponent& InComponent)
{
  const SpawnEntityComponent* component = reinterpret_cast<const SpawnEntityComponent*>(&InComponent);
  const Entity* entitySpawned = AddEntity(component->entityTypeHash, component->entityId);

  std::cout << "Spawn entity received! " << entitySpawned->GetTypeTag().ToStr() << "\n";
}

void EntityManager::OnEntityDespawnReceived(const sockaddr_storage& InTarget, const Net::PacketComponent& InComponent)
{
  const DeSpawnEntityComponent* component = reinterpret_cast<const DeSpawnEntityComponent*>(&InComponent);
  RemoveEntity(component->entityId);
}

void EntityManager::OnEntitySpawnRequestReceived(const sockaddr_storage& InTarget, const Net::PacketComponent& InComponent)
{
  const RequestSpawnEntityComponent* component = reinterpret_cast<const RequestSpawnEntityComponent*>(&InComponent);
  // TODO: Nothing at the moment, needs security if implemented
}

void EntityManager::OnEntityDespawnRequestReceived(const sockaddr_storage& InTarget, const Net::PacketComponent& InComponent)
{
  const RequestDeSpawnEntityComponent* component = reinterpret_cast<const RequestDeSpawnEntityComponent*>(&InComponent);
  // TODO: Nothing at the moment, needs security if implemented
}

void EntityManager::OnConnectionReceived(const sockaddr_storage& InTarget, const Net::PacketComponent& InComponent)
{
  const NetTag playerTypeTag = NetTag("player.ship");
  PlayerShipEntity* entitySpawned = static_cast<PlayerShipEntity*>(SpawnEntityServer(playerTypeTag));
  entitySpawned->GetComponent<ControllerComponent>()->SetPossessedBy(InTarget);

  SetEntityPossessedComponent setEntityPossessed;
  setEntityPossessed.entityIdentifier = entitySpawned->GetId();
  setEntityPossessed.bShouldPossess = true;
  Net::PacketManager::Get()->SendPacketComponent<SetEntityPossessedComponent>(setEntityPossessed, InTarget);
  
  std::cout << "Join!" << "\n";
}

void EntityManager::OnInputReceived(const sockaddr_storage& InTarget, const Net::PacketComponent& InComponent)
{
  const InputComponent* component = reinterpret_cast<const InputComponent*>(&InComponent);
  if (entities_.find(component->entityIdentifier) != entities_.end())
  {
    Entity* entity = entities_.at(component->entityIdentifier).get();
    ControllerComponent* controllerComponent = entity->GetComponent<ControllerComponent>();

    // Check if target entity is possessed by the client
    if (controllerComponent && controllerComponent->IsPossessedBy(InTarget))
    {
      // TODO: Update input with rollback and forward system to get accurate input from packet delays. Rollback a something like 5 frames depending on lag.

      const float deltaTime = Net::PacketManager::Get()->GetDeltaTime();
      const float xVelocity = component->xAxis * controllerComponent->GetSpeed() * deltaTime;
      const float yVelocity = component->yAxis * controllerComponent->GetSpeed() * deltaTime;
      
      UpdateEntityPositionComponent updateEntityPositionComponent;
      updateEntityPositionComponent.entityIdentifier = entity->GetId();
      updateEntityPositionComponent.xPos = entity->GetPosition().x + xVelocity;
      updateEntityPositionComponent.yPos = entity->GetPosition().y + yVelocity;
      Net::PacketManager::Get()->SendPacketComponentMulticast<UpdateEntityPositionComponent>(updateEntityPositionComponent);

      std::cout << "Received Velocity " << xVelocity << " : " << yVelocity << " : " << deltaTime << "\n";
      
      // Update location on server
      controllerComponent->UpdatePosition(updateEntityPositionComponent.xPos, updateEntityPositionComponent.yPos);

      // Update net position for culling
      Net::PacketManager::Get()->UpdateClientNetPosition(InTarget, { entity->GetPosition().x, entity->GetPosition().y, 0.f });
    }
  }
}

void EntityManager::OnPositionUpdateReceived(const sockaddr_storage& InTarget, const Net::PacketComponent& InComponent)
{
  const UpdateEntityPositionComponent* component = reinterpret_cast<const UpdateEntityPositionComponent*>(&InComponent);
  if (entities_.find(component->entityIdentifier) != entities_.end())
  {
    Entity* entity = entities_.at(component->entityIdentifier).get();
    ControllerComponent* controllerComponent = entity->GetComponent<ControllerComponent>();
    
    if (controllerComponent)
    {
      controllerComponent->UpdatePosition(component->xPos, component->yPos);

      // Debug for client self position
      if (entity == possessedEntity_)
      {
        std::cout << "Updated Position " << component->xPos << " : " << component->yPos << "\n";
      }
    }
  }
}

void EntityManager::OnSetEntityPossessedReceived(const sockaddr_storage& InTarget, const Net::PacketComponent& InComponent)
{
  const SetEntityPossessedComponent* component = reinterpret_cast<const SetEntityPossessedComponent*>(&InComponent);
  entityToUpdatePossess_ = static_cast<int16_t>(component->entityIdentifier);
  bPossession_ = component->bShouldPossess;
}

void EntityManager::OnReturnAckReceived(const sockaddr_storage& InTarget, const Net::PacketComponent& InComponent)
{
  const ReturnAckComponent* component = reinterpret_cast<const ReturnAckComponent*>(&InComponent);
}

Entity* EntityManager::AddEntity(const uint64_t InEntityTypeHash, const uint16_t InIdentifier)
{
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
  
  entities_.insert({ InIdentifier, newEntity });
  
  return newEntity.get();
}

bool EntityManager::RemoveEntity(const uint16_t InIdentifier)
{
  if (entities_.find(InIdentifier) != entities_.end())
  {
    entities_.erase(InIdentifier);
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
  const PacketComponentAssociatedData associatedDataSpawnDeSpawnComps = PacketComponentAssociatedData
  {
    false,
    0.5f,
    EPacketHandlingType::Ack
  };
  
  Net::PacketManager::Get()->RegisterPacketComponent<DeSpawnEntityComponent, EntityManager>(associatedDataSpawnDeSpawnComps, &EntityManager::OnEntityDespawnReceived, this);
  Net::PacketManager::Get()->RegisterPacketComponent<SpawnEntityComponent, EntityManager>(associatedDataSpawnDeSpawnComps, &EntityManager::OnEntitySpawnReceived, this);
  Net::PacketManager::Get()->RegisterPacketComponent<RequestDeSpawnEntityComponent, EntityManager>(associatedDataSpawnDeSpawnComps, &EntityManager::OnEntityDespawnRequestReceived, this);
  Net::PacketManager::Get()->RegisterPacketComponent<RequestSpawnEntityComponent, EntityManager>(associatedDataSpawnDeSpawnComps, &EntityManager::OnEntitySpawnRequestReceived, this);

  Net::PacketManager::Get()->RegisterPacketComponent<SetEntityPossessedComponent, EntityManager>(associatedDataSpawnDeSpawnComps, &EntityManager::OnSetEntityPossessedReceived, this);

  const PacketComponentAssociatedData associatedDataEveryTick = PacketComponentAssociatedData
  {
    true,
    FIXED_UPDATE_TIME,
    EPacketHandlingType::None
  };
  Net::PacketManager::Get()->RegisterPacketComponent<InputComponent, EntityManager>(associatedDataEveryTick, &EntityManager::OnInputReceived, this);
  Net::PacketManager::Get()->RegisterPacketComponent<UpdateEntityPositionComponent, EntityManager>(associatedDataEveryTick, &EntityManager::OnPositionUpdateReceived, this);
}

void EntityManager::SubscribeToPacketComponents()
{
  Net::PacketComponentDelegator& componentDelegator = Net::PacketManager::Get()->GetPacketComponentDelegator();

  componentDelegator.SubscribeToPacketComponentDelegate<ServerConnectPacketComponent, EntityManager>(&EntityManager::OnConnectionReceived, this);
  componentDelegator.SubscribeToPacketComponentDelegate<ReturnAckComponent, EntityManager>(&EntityManager::OnReturnAckReceived, this);
}