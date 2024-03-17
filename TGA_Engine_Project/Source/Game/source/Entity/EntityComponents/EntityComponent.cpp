#include "EntityComponent.h"

#include "../Entities/Entity.h"

EntityComponent::EntityComponent()
{
	
}

void EntityComponent::UpdateReplication()
{
	// Wait with replication until has been received
	if (!bHasBeenReceived_)
	{
		return;
	}
	
	UpdateReplicationPacketComponent();
	
	if (oldReplicationPacketComponent_.variableDataObject != replicationPacketComponent_.variableDataObject)
	{
		// Cache and erase already updated variable data
		VariableDataObject<REPLICATION_DATA_SIZE> temporaryVariableDataObject = replicationPacketComponent_.variableDataObject;
		temporaryVariableDataObject.EraseEqualMemberVariableData(oldReplicationPacketComponent_.variableDataObject);

		if (temporaryVariableDataObject.GetTotalSize() > 0)
		{
			oldReplicationPacketComponent_ = replicationPacketComponent_; // Replace old cached data
			
			replicationPacketComponent_.variableDataObject = temporaryVariableDataObject; // Set modified data with already up to date variables removed
			replicationPacketComponent_.UpdateSize();
			
			Net::PacketManager::Get()->SendPacketComponentMulticast<DataReplicationPacketComponent>(replicationPacketComponent_);
		}
	}
}

void EntityComponent::UpdateReplicationForTarget(const sockaddr_storage& InAddress)
{
	// Wait with replication until has been received
	if (!bHasBeenReceived_)
	{
		return;
	}
	
	if (replicationPacketComponent_.GetSize() > REPLICATION_COMPONENT_SIZE_EMPTY + VARIABLE_DATA_OBJECT_DEFAULT_SIZE)
	{
		Net::PacketManager::Get()->SendPacketComponent<DataReplicationPacketComponent>(replicationPacketComponent_, InAddress);
	}
}

void EntityComponent::UpdateReplicationPacketComponent()
{
	replicationPacketComponent_.Reset();
	replicationPacketComponent_.identifierDataFirst = owner_->GetId();
	replicationPacketComponent_.identifierDataSecond = id_;
	OnSendReplication(replicationPacketComponent_);
	replicationPacketComponent_.UpdateSize();
}
