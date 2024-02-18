#pragma once

#include "../PacketComponent.h"

class NET_LIB_EXPORT SuccessfullyConnectedToServer : public Net::PacketComponent
{
public:
  SuccessfullyConnectedToServer();
};

inline SuccessfullyConnectedToServer::SuccessfullyConnectedToServer()
    : PacketComponent(static_cast<int16_t>(Net::EPacketComponent::SuccessfullyConnectedToServer), sizeof(SuccessfullyConnectedToServer))
{ }