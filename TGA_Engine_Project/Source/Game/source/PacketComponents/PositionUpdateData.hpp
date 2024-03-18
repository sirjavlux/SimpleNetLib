#pragma once

#include "NetIncludes.h"

struct PositionUpdateData
{
	void SetPosition(const Tga::Vector2f& InPosition)
	{
		xPos_ = static_cast<int16_t>(InPosition.x * 1000.f);
		yPos_ = static_cast<int16_t>(InPosition.y * 1000.f);
	}

	Tga::Vector2f GetPosition() const
	{
		return { static_cast<float>(xPos_) / 1000.f, static_cast<float>(yPos_) / 1000.f };
	}

	void SetDirection(const Tga::Vector2f& InDirection)
	{
		direction_ = static_cast<int8_t>(std::atan2f(InDirection.y, InDirection.x) * 10.f);
	}

	float GetDirectionAsAngle() const
	{
		return static_cast<float>(direction_) / 10.f;
	}

	Tga::Vector2f GetDirection() const
	{
		const float angleAsFloat = GetDirectionAsAngle();

		Tga::Vector2f direction;
		direction.x = std::cosf(angleAsFloat);
		direction.y = std::sinf(angleAsFloat);

		return direction;
	}

	bool bIsTeleport = true;
	
private:
	int8_t direction_ = 0;

	// This would have to be increased if the map is bigger then (-16 - 16) x (-16 - 16)
	int16_t xPos_ = 0;
	int16_t yPos_ = 0;
};