#include "Replicable.h"

#include "Packet/PacketManager.h"

void Replicable::UpdateReplication(const uint16_t InFirstAddress, const uint16_t InSecondAddress)
{
	// Wait with replication until has been received
	if (!bHasBeenReceived_)
	{
		return;
	}
	
	UpdateReplicationPacketComponent(InFirstAddress, InSecondAddress);
	
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

	replicationPacketComponent_.Reset();
}

void Replicable::UpdateReplicationForTarget(const sockaddr_storage& InAddress, const uint16_t InFirstAddress, const uint16_t InSecondAddress)
{
	// Wait with replication until has been received
	if (!bHasBeenReceived_)
	{
		return;
	}

	UpdateReplicationPacketComponent(InFirstAddress, InSecondAddress);
	
	if (replicationPacketComponent_.GetSize() > REPLICATION_COMPONENT_SIZE_EMPTY + VARIABLE_DATA_OBJECT_DEFAULT_SIZE)
	{
		Net::PacketManager::Get()->SendPacketComponent<DataReplicationPacketComponent>(replicationPacketComponent_, InAddress);
	}

	replicationPacketComponent_.Reset();
}

void Replicable::UpdateReplicationPacketComponent(const uint16_t InFirstAddress, const uint16_t InSecondAddress)
{
	replicationPacketComponent_.identifierDataFirst = InFirstAddress;
	replicationPacketComponent_.identifierDataSecond = InSecondAddress;
	OnSendReplication(replicationPacketComponent_);
	replicationPacketComponent_.UpdateSize();
}