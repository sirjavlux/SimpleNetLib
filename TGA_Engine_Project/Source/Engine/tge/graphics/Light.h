#pragma once
#include <tge/Math/Color.h>
#include <tge/Math/Transform.h>
#include <tge/Math/Vector.h>

namespace Tga
{
class Light
{
protected:
	Color myColor;
	float myIntensity;
	Transform myTransform;

public:

	virtual ~Light() = default;
	
	Light(const Transform& someTransform, const Color& aColor, float aIntensity);

	FORCEINLINE float GetIntensity() const { return myIntensity; }
	FORCEINLINE Color GetColor() const { return myColor; }

	FORCEINLINE Transform GetTransform() const { return myTransform; }
	FORCEINLINE void SetTransform(Transform newTransform) { myTransform = newTransform; }
	FORCEINLINE void SetPosition(Vector3f newPosition) { myTransform.SetPosition(newPosition); }
	FORCEINLINE void SetRotation(Rotator newRotation) { myTransform.SetRotation(newRotation); }
	FORCEINLINE void SetColor(Color newColor) { myColor = newColor; }
	FORCEINLINE void SetIntensity(float newIntensity) { myIntensity = newIntensity; }

};

} // namespace Tga