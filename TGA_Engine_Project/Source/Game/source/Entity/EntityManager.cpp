#include "stdafx.h"
#include "EntityManager.h"

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

void EntityManager::RequestDestroyEntity(uint16_t InIdentifier)
{
  
}

void EntityManager::DestroyEntity(const uint16_t InIdentifier)
{
  if (!IsServer())
  {
    return;
  }

  
}

bool EntityManager::IsServer()
{
  return Net::PacketManager::Get()->GetManagerType() == ENetworkHandleType::Server;
}

void EntityManager::OnEntitySpawnReceived(const sockaddr_storage& InTarget, const Net::PacketComponent& InComponent)
{
  
}

void EntityManager::OnEntityDespawnReceived(const sockaddr_storage& InTarget, const Net::PacketComponent& InComponent)
{
  
}

void EntityManager::OnEntitySpawnRequestReceived(const sockaddr_storage& InTarget, const Net::PacketComponent& InComponent)
{
  
}

void EntityManager::OnEntityDespawnRequestReceived(const sockaddr_storage& InTarget, const Net::PacketComponent& InComponent)
{
  
}

uint16_t EntityManager::GenerateEntityIdentifier()
{
  static uint16_t identifierIter = 0;
  return ++identifierIter;
}

void EntityManager::RegisterPacketComponents()
{
  
}
