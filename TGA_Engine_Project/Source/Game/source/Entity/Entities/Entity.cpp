#include "Entity.h"

#include "../../PacketComponents/UpdateEntityPositionComponent.hpp"
#include "Packet/PacketManager.h"

void Entity::NativeOnDestruction()
{
	OnDestruction();
}

void Entity::UpdateComponents(const float InDeltaTime)
{
	for (const auto& component : componentsMap_)
	{
		component.second->Update(InDeltaTime);
	}
}

void Entity::FixedUpdateComponents()
{
	for (const auto& component : componentsMap_)
	{
		component.second->FixedUpdate();
	}
}

void Entity::NativeFixedUpdate()
{
	if (!bIsLocalEntity_ && Net::PacketManager::Get()->IsServer())
	{
		UpdateReplication();
	}
}

void Entity::SetPosition(const Tga::Vector2f& InPos, const bool InIsTeleport)
{
	position_ = InPos;
	if (InIsTeleport || bIsStatic_)
	{
		SetTargetPosition(InPos);
	}
}

std::weak_ptr<EntityComponent> Entity::GetComponentById(const uint16_t InIdentifier)
{
	if (componentsMap_.find(InIdentifier) != componentsMap_.end())
	{
		return componentsMap_.at(InIdentifier);
	}

	return std::weak_ptr<EntityComponent>();
}

void Entity::UpdateReplication()
{
	// Wait with replication until has been received
	if (!bHasBeenReceived_)
	{
		return;
	}
	
	// Custom replication
	replicationPacketComponent_.Reset();
	replicationPacketComponent_.identifierDataFirst = id_;
	OnSendReplication(replicationPacketComponent_);
	replicationPacketComponent_.UpdateSize();
	if (replicationPacketComponent_.GetSize() > REPLICATION_COMPONENT_SIZE_EMPTY + VARIABLE_DATA_OBJECT_DEFAULT_SIZE)
	{
		Net::PacketManager::Get()->SendPacketComponentMulticast<DataReplicationPacketComponent>(replicationPacketComponent_);
	}

	// Update Custom Replication for Components
	for (auto& component : componentsMap_)
	{
		component.second->UpdateReplication();
	}
	
	// Replicate position if enabled
	if (bShouldReplicatePosition_)
	{
		UpdateEntityPositionComponent positionComponent;
		positionComponent.bIsTeleport = false;
		positionComponent.xPos = targetPosition_.x;
		positionComponent.yPos = targetPosition_.y;
		positionComponent.rotation = std::atan2(direction_.y, direction_.x);
		positionComponent.entityIdentifier = GetId();
		positionComponent.SetOverrideDefiningData(GetId());
		Net::PacketManager::Get()->SendPacketComponentMulticastWithLod<UpdateEntityPositionComponent>(positionComponent, { targetPosition_.x, targetPosition_.y, 0.f });
	}
}