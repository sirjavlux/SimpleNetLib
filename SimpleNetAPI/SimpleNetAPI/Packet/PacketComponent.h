#pragma once

enum class EPacketComponentType
{
	None		= 0,
	Ack			= 1,
	AckReturn	= 2,
};

class PacketComponent
{
public:
	PacketComponent();

protected:
	unsigned int mComponentIdentifier = -1;

	EPacketComponentType mComponentType = EPacketComponentType::None;


};