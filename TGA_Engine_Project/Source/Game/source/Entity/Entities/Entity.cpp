#include "Entity.h"

#include "../../PacketComponents/UpdateEntityPositionComponent.hpp"
#include "Packet/PacketManager.h"

Entity::Entity()
{
	customAssociatedDataReplication_.packetFrequencySeconds = 0.2f;

	std::vector<std::pair<float, float>> packetLodFrequencies;
	packetLodFrequencies.emplace_back(1.0f, 0.08f);
	packetLodFrequencies.emplace_back(1.2f, 0.2f);
	packetLodFrequencies.emplace_back(1.8f, 1.f);
	packetLodFrequencies.emplace_back(3.0f, 3.f);
	
	customAssociatedDataMovementReplication_.packetFrequencySeconds = 0.08f;
	customAssociatedDataMovementReplication_.packetLodFrequencies = packetLodFrequencies;
	//customAssociatedDataMovementReplication_.distanceToCullPacketComponentAt = 1.4f;
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

	return {};
}

void Entity::UpdateReplication()
{
	// Wait with replication until has been received
	if (!bHasBeenReceived_)
	{
		return;
	}
	
	// Custom replication
	uint8_t frequency;
	if (bShouldUseCustomReplicationData_)
	{
		frequency = Net::PacketTargetData::FromPacketComponentSendFrequencySecondsToTicks(customAssociatedDataReplication_.packetFrequencySeconds);
	} else
	{
		const uint16_t componentIdentifier = replicationPacketComponent_.GetIdentifier();
		const PacketComponentAssociatedData* packetComponentSettings = Net::SimpleNetLibCore::Get()->GetPacketComponentRegistry()->FetchPacketComponentAssociatedData(componentIdentifier);
		frequency = Net::PacketTargetData::FromPacketComponentSendFrequencySecondsToTicks(packetComponentSettings->packetFrequencySeconds);
	}
	
	if (Net::PacketManager::Get()->GetUpdateIterator() % frequency == 0)
	{
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
	}

	// TODO: Something is wrong with the culling here, if culled, the player can still see the object remaining, standing still. Possible solution, disable rendering
	// Replicate position if enabled
	if (bShouldReplicatePosition_)
	{
		const std::unordered_map<sockaddr_in, Net::NetTarget>& connections = Net::SimpleNetLibCore::Get()->GetNetHandler()->GetNetConnectionHandler().GetConnections();
		for (const auto& connection : connections)
		{
			const float distanceSqr = NetUtility::NetVector3{ targetPosition_.x, targetPosition_.y, 0.f }.DistanceSqr(connection.second.netCullingPosition);
			const PacketComponentAssociatedData* packetComponentSettings = bShouldUseCustomMovementReplicationData_ ? &customAssociatedDataMovementReplication_
				: Net::SimpleNetLibCore::Get()->GetPacketComponentRegistry()->FetchPacketComponentAssociatedData(replicationPacketComponent_.GetIdentifier());
			
			if (!(packetComponentSettings->distanceToCullPacketComponentAt > 0 && packetComponentSettings->distanceToCullPacketComponentAt < distanceSqr)
				&& Net::PacketManager::Get()->GetUpdateIterator() % Net::PacketTargetData::GetLodedFrequency(packetComponentSettings, distanceSqr) == 0)
			{
				UpdateEntityPositionComponent positionComponent;
				positionComponent.positionUpdateData.bIsTeleport = false;
				positionComponent.positionUpdateData.SetPosition(targetPosition_);
				positionComponent.positionUpdateData.SetDirection(direction_);
				positionComponent.entityIdentifier = GetId();
				positionComponent.SetOverrideDefiningData(GetId());

				Net::PacketManager::Get()->SendPacketComponent<UpdateEntityPositionComponent>(positionComponent, NetUtility::RetrieveStorageFromIPv4Address(connection.first));
			}
		}
	}
}