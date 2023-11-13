#pragma once

#include <tge/Math/Matrix4x4.h>
#include <tge/Math/Transform.h>

namespace Tga
{

class Camera
{
private:

	Transform myTransform{};
	
	Matrix4x4f myProjection{};
	float myNearPlane = 0.01f;
	float myFarPlane = 1500.0f;
	
public:
	Camera();
	~Camera();
	
	void SetOrtographicProjection(float aWidth, float aHeight, float aDepth);
	void SetOrtographicProjection(float aLeft, float aRight, float aTop, float aBottom, float aNear, float aFar);
	void SetPerspectiveProjection(float aHorizontalFoV, Vector2f aResolution, float aNearPlane, float aFarPlane);

	void SetTransform(Vector3f aPosition, Vector3f aRotation);
	void SetTransform(Transform someTransform);
	void SetRotation(Rotator aRotation);
	void SetPosition(Vector3f aPosition);

	Transform GetTransform() const { return myTransform; }
	Matrix4x4f GetProjection() const { return myProjection; }
	void GetProjectionPlanes(float& aNearPlane, float& aFarPlane) const { aNearPlane = myNearPlane; aFarPlane = myFarPlane; }
};

} // namespace Tga
