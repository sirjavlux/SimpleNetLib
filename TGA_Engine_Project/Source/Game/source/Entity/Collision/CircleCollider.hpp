#pragma once

#include "Collider.hpp"
#include "..\Entities\Entity.h"

struct CircleCollider : Collider
{
	CircleCollider() : radius(0.f)
	{
		type = EColliderType::Circle;
	}

	float radius;
};

namespace Collision
{
	static bool IntersectsCircleToCircle(CircleCollider& LhsCollider, CircleCollider& RhsCollider)
	{
		const float distanceSqr = ((LhsCollider.owner->GetPosition() + LhsCollider.localPosition)
			- (RhsCollider.owner->GetPosition() + RhsCollider.localPosition)).LengthSqr();
		
		const float radiusSqrLhs = LhsCollider.radius * LhsCollider.radius;
		const float radiusSqrRhs = RhsCollider.radius * RhsCollider.radius;
		
		return distanceSqr < radiusSqrLhs + radiusSqrRhs;
	}

	static bool Intersects(Collider& LhsCollider, Collider& RhsCollider)
	{
		if (LhsCollider.type == EColliderType::Circle && RhsCollider.type == EColliderType::Circle)
		{
			return IntersectsCircleToCircle(dynamic_cast<CircleCollider&>(LhsCollider), dynamic_cast<CircleCollider&>(RhsCollider));
		}

		// TODO: Add more collision cases...
		
		return false;
	}
}
