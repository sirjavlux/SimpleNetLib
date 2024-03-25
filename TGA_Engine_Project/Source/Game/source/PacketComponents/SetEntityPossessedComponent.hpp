#pragma once

#include "PacketComponentTypes.hpp"
#include "Packet/PacketComponent.h"
#include "../Definitions.hpp"

class SetEntityPossessedComponent : public Net::PacketComponent
{
public:
	SetEntityPossessedComponent();

	char usernameBuffer[USERNAME_MAX_LENGTH];
	
	uint16_t entityIdentifier = 0;
	sockaddr_in possessor;
	
	bool bShouldPossess = false;
};

inline SetEntityPossessedComponent::SetEntityPossessedComponent()
    : PacketComponent(static_cast<int16_t>(EPacketComponentType::SetEntityPossessed), sizeof(SetEntityPossessedComponent))
{ }