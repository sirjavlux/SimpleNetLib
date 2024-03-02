#pragma once

#include "PacketComponentTypes.hpp"
#include "../Entity/EntityComponents/ControllerComponent.h"
#include "Packet/PacketComponent.h"

enum class EKeyInput : uint16_t
{
	None = 0,

	Space = 1 << 0,
	Shift = 1 << 1,
	
	W = 1 << 2,
	S = 1 << 3,
	A = 1 << 4,
	D = 1 << 5,
	Q = 1 << 6,
	E = 1 << 7,
};

inline EKeyInput operator|(EKeyInput InLhs, EKeyInput InRhs)
{
	return static_cast<EKeyInput>(static_cast<uint16_t>(InLhs) | static_cast<uint16_t>(InRhs));
}

inline EKeyInput operator&(EKeyInput InLhs, EKeyInput InRhs)
{
	return static_cast<EKeyInput>(static_cast<uint16_t>(InLhs) & static_cast<uint16_t>(InRhs));
}

inline EKeyInput operator|=(EKeyInput InLhs, EKeyInput InRhs)
{
	return static_cast<EKeyInput>(static_cast<uint16_t>(InLhs) | static_cast<uint16_t>(InRhs));
}

inline EKeyInput operator&=(EKeyInput InLhs, EKeyInput InRhs)
{
	return static_cast<EKeyInput>(static_cast<uint16_t>(InLhs) & static_cast<uint16_t>(InRhs));
}

class InputComponent : public Net::PacketComponent
{
public:
	InputComponent();
	
	uint16_t entityIdentifier = 0;
	uint16_t keysPressBuffer = 0; 
	uint32_t sequenceNr = 0;
};

inline InputComponent::InputComponent()
    : PacketComponent(static_cast<int16_t>(EPacketComponentType::Input), sizeof(InputComponent))
{ }