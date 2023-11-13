#pragma once
#include "Light.h"

namespace Tga
{
class DirectionalLight : public Light
{
public:
	DirectionalLight(const Transform& someTransform, const Color& aColor, float aIntensity) : Light(someTransform, aColor, aIntensity) {}
	virtual ~DirectionalLight() = default;
};

} // namespace Tga

