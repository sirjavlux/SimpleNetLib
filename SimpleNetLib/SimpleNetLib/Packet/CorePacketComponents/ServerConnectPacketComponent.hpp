#pragma once

#include "../PacketComponent.h"
#include "../../Utility/VariableDataObject.hpp"

class NET_LIB_EXPORT ServerConnectPacketComponent : public Net::PacketComponent
{
public:
    void SetVariableData(const VariableDataObject<CONNECTION_DATA_SIZE>& InVariableData)
    {
        variableDataObject = InVariableData;
        sizeData_ = DEFAULT_PACKET_COMPONENT_SIZE + variableDataObject.GetTotalSizeOfObject();
    }
    
    VariableDataObject<CONNECTION_DATA_SIZE> variableDataObject;

    ServerConnectPacketComponent::ServerConnectPacketComponent()
    : PacketComponent(static_cast<int16_t>(Net::EPacketComponent::ServerConnect), CONNECTION_COMPONENT_SIZE_EMPTY)
    { }
};
