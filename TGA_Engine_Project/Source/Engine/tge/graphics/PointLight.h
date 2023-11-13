#pragma once
#include "Light.h"

namespace Tga
{

class PointLight : public Light
{
protected:
	float myRange = 10.0f;
	
public:
	PointLight(const Transform& someTransform, const Color& aColor, float aIntensity, float someRange) : Light(someTransform, aColor, aIntensity), myRange(someRange) {}
	virtual ~PointLight() = default;

	void SetRange(float someRange);
	FORCEINLINE float GetRange() const { return myRange; }
};

} // namespace Tga