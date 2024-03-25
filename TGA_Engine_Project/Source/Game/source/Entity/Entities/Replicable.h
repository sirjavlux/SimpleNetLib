#pragma once
#include "Packet/CorePacketComponents/DataReplicationPacketComponent.hpp"

class Replicable
{
protected:
	// Server Sided
	virtual void OnSendReplication(DataReplicationPacketComponent& OutComponent) {}
	// Client Sided
	virtual void OnReadReplication(const DataReplicationPacketComponent& InComponent) {}

	bool HasBeenReceivedByClient() const { return bHasBeenReceived_; }

	void UpdateReplication(uint16_t InFirstAddress, uint16_t InSecondAddress);
	void UpdateReplicationForTarget(const sockaddr_storage& InAddress, uint16_t InFirstAddress, uint16_t InSecondAddress);

	VariableDataObject<REPLICATION_DATA_SIZE>& GetVariableDataObject() { return replicationPacketComponent_.variableDataObject; }
	
private:
	void UpdateReplicationPacketComponent(uint16_t InFirstAddress, uint16_t InSecondAddress);
  
	DataReplicationPacketComponent replicationPacketComponent_;
	DataReplicationPacketComponent oldReplicationPacketComponent_;

	bool bHasBeenReceived_ = false;

	friend class EntityManager;
};