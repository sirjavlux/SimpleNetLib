#include "Entity.h"

#include "../../PacketComponents/UpdateEntityPositionComponent.hpp"
#include "Packet/PacketManager.h"

Entity::Entity()
{
	customAssociatedDataReplication_.packetFrequency = 18;

	std::vector<std::pair<float, uint8_t>> packetLodFrequencies;
	packetLodFrequencies.emplace_back(1.0f, 3);
	packetLodFrequencies.emplace_back(1.2f, 18);
	packetLodFrequencies.emplace_back(1.8f, 60);
	packetLodFrequencies.emplace_back(3.0f, 180);
	
	customAssociatedDataMovementReplication_.packetFrequency = 3;
	customAssociatedDataMovementReplication_.packetLodFrequencies = packetLodFrequencies;
}

void Entity::NativeOnDestruction()
{
	OnDestruction();
	for (auto& component : componentsMap_)
	{
		component.second->OnDestruction();
	}
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
		UpdateReplicationEntity();
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

	return {};
}

void Entity::UpdateReplicationEntity()
{
	// Wait with replication until has been received
	if (!HasBeenReceivedByClient())
	{
		return;
	}
	
	// Custom replication
	uint8_t frequency;
	if (bShouldUseCustomReplicationData_)
	{
		frequency = customAssociatedDataReplication_.packetFrequency;
	}
	else
	{
		const uint16_t componentIdentifier = replicationPacketComponent_.GetIdentifier();
		const PacketComponentAssociatedData* packetComponentSettings = Net::SimpleNetLibCore::Get()->GetPacketComponentRegistry()->FetchPacketComponentAssociatedData(componentIdentifier);
		frequency = packetComponentSettings->packetFrequency;
	}
	
	if (Net::PacketManager::Get()->GetUpdateIterator() % frequency == 0)
	{
		GetVariableDataObject().SerializeMemberVariable(*this, parentEntity_);
		UpdateReplication(id_, 0);

		// Update Custom Replication for Components
		for (auto& component : componentsMap_)
		{
			component.second->UpdateReplication(id_, component.second->id_);
		}
	}
	
	// Replicate position if enabled
	if (bShouldReplicatePosition_)
	{
		const std::unordered_map<sockaddr_in, Net::NetTarget>& connections = Net::SimpleNetLibCore::Get()->GetNetHandler()->GetNetConnectionHandler().GetConnections();
		for (const auto& connection : connections)
		{
			const float distanceSqr = NetUtility::NetVector3{ targetPosition_.x, targetPosition_.y, 0.f }.DistanceSqr(connection.second.netCullingPosition);
			const PacketComponentAssociatedData* packetComponentSettings = bShouldUseCustomMovementReplicationData_ ? &customAssociatedDataMovementReplication_
				: Net::SimpleNetLibCore::Get()->GetPacketComponentRegistry()->FetchPacketComponentAssociatedData(replicationPacketComponent_.GetIdentifier());

			const uint8_t loddedFrequency = Net::PacketTargetData::GetLodedFrequency(packetComponentSettings, distanceSqr);
			if (!(packetComponentSettings->distanceToCullPacketComponentAt > 0 && packetComponentSettings->distanceToCullPacketComponentAt < distanceSqr)
				&& Net::PacketManager::Get()->GetUpdateIterator() % loddedFrequency == 0)
			{
				UpdateEntityPositionComponent positionComponent;
				positionComponent.positionUpdateData.bIsTeleport = false;
				positionComponent.positionUpdateData.SetPosition(targetPosition_);
				positionComponent.positionUpdateData.SetDirection(direction_);
				positionComponent.entityIdentifier = GetId();
				positionComponent.SetOverrideDefiningData(GetId());

				Net::PacketManager::Get()->SendPacketComponentWithLod<UpdateEntityPositionComponent>(positionComponent, { targetPosition_.x, targetPosition_.y, 0.f }, NetUtility::RetrieveStorageFromIPv4Address(connection.first), packetComponentSettings);
			}
		}
	}
}