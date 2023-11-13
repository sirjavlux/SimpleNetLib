#pragma once
#include "stdafx.h"

#include <queue>

enum class EntityType : uint8_t
{
    PlayerSelected,
    Player,
    Flower,
	AnimatedObject,
};

class Entity
{
public:
	virtual ~Entity() { }

	virtual void Init() = 0;
	virtual void InitServer() = 0;
	virtual void Update(float aDeltaTime) = 0;
	virtual void Render() = 0;

	inline void SetPosition(const Tga::Vector2f& aPos) { myTargetPos = aPos; myPos = aPos; }
	inline Tga::Vector2f GetPosition() const { return myPos; }

	inline void SetVelocity(const Tga::Vector2f& aVel) { myVel = aVel; }
	inline Tga::Vector2f GetVelocity() const { return myVel; }

	inline short GetID() { return myID; }

	inline float GetSpeed() { return mySpeed; }

protected:

	int myLODModuleHandle = -1;

	Tga::Vector2f myVel;
	Tga::Vector2f myTargetPos;
	Tga::Vector2f myPos;

	short myID = 0;

	float mySpeed = 0.1f;

	EntityType myType;
};