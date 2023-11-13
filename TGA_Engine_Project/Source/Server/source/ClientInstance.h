#pragma once

#include "Natverk/Network-Shared.h"
#include "Natverk/PacketManager.h"

class Player;
class PacketManager;

class ClientInstance
{
public:
	ClientInstance();
	~ClientInstance();
	
	Player* myPlayer;
	Net::AddressSerializedData myAddress;

	float myTimeSinceLastPacket = 0.f;

	bool myShouldErase = false;
};