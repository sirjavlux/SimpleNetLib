#pragma once

#include "../PacketComponent.h"

class NET_LIB_EXPORT SuccessfullyConnectedToServer : public Net::PacketComponent
{
public:
	SuccessfullyConnectedToServer();

	void SetVariableData(const VariableDataObject<CONNECTION_DATA_SIZE>& InVariableData)
	{
		variableDataObject = InVariableData;
		sizeData_ = DEFAULT_PACKET_COMPONENT_SIZE + variableDataObject.dataIter;
	}
    
	VariableDataObject<CONNECTION_DATA_SIZE> variableDataObject;
};

inline SuccessfullyConnectedToServer::SuccessfullyConnectedToServer()
    : PacketComponent(static_cast<int16_t>(Net::EPacketComponent::SuccessfullyConnectedToServer), sizeof(SuccessfullyConnectedToServer))
{ }