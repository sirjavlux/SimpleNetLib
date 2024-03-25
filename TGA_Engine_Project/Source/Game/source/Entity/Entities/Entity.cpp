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
		frequency = Net::PacketTargetData::FromPacketComponentSendFrequencySecondsToTicks(customAssociatedDataReplication_.packetFrequencySeconds);
	}
	else
	{
		const uint16_t componentIdentifier = replicationPacketComponent_.GetIdentifier();
		const PacketComponentAssociatedData* packetComponentSettings = Net::SimpleNetLibCore::Get()->GetPacketComponentRegistry()->FetchPacketComponentAssociatedData(componentIdentifier);
		frequency = Net::PacketTargetData::FromPacketComponentSendFrequencySecondsToTicks(packetComponentSettings->packetFrequencySeconds);
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