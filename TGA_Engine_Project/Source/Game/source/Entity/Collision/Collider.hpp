#pragma once
#include "tge/math/Vector2.h"

class Entity;

enum class EColliderType : uint8_t
{
	None	= 0,
	Circle	= 1,
};

struct Collider
{
	virtual ~Collider() = default;
	
	EColliderType type;
	Tga::Vector2f localPosition;
	Tga::Vector2f oldPosition;
	Entity* owner;
};
