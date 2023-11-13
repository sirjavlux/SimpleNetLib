#include "stdafx.h"
#include "Camera.h"
#include <iostream>

using namespace Tga;

Camera::Camera()
{}

Camera::~Camera()
{}

void Camera::SetOrtographicProjection(float aWidth, float aHeight, float aDepth)
{
	myProjection = {};

	myProjection(1, 1) = 2.f/aWidth;
	myProjection(2, 2) = 2.f/aHeight;
	myProjection(3, 3) = 1.f/aDepth;
}

void Camera::SetOrtographicProjection(float aLeft, float aRight, float aTop, float aBottom, float aNear, float aFar)
{
	myProjection = {};

	myProjection(1, 1) = 2.f / (aRight - aLeft);
	myProjection(2, 2) = 2.f / (aBottom - aTop);
	myProjection(3, 3) = 1.f / (aFar - aNear);

	myProjection(4, 1) = -(aRight + aLeft) / (aRight - aLeft);
	myProjection(4, 2) = -(aBottom + aTop) / (aBottom - aTop);
	myProjection(4, 3) = -(aNear) / (aFar - aNear);
}

void Camera::SetPerspectiveProjection(float aHorizontalFoV, Vector2f aResolution, float aNearPlane, float aFarPlane)
{
	myProjection = {};

	assert(aNearPlane < aFarPlane);
	assert(aNearPlane >= FMath::KindaSmallNumber);
	
	myNearPlane = aNearPlane;
	myFarPlane = aFarPlane;
	
	// aHorizontalFoV is in Degrees!
	// Convert to Radians
	const float hFoVRad = aHorizontalFoV * (FMath::Pi / 180);

	// Then we figure out the ideal vertical FoV based on the screen or window resolution.
	// You can pass a resolution, i.e. 1920x1080, 5120x1440, or aspects such as 16x9, 32x9.
	const float vFoVRad = 2 * std::atan(std::tan(hFoVRad / 2) * (aResolution.Y / aResolution.X));

	// If we wanted to return it to degrees we could do this.
	// Will be ~59 if resolution is a 16:9 aspect like 1920x1080.
	//float fFoVDeg = std::ceil(vFoVRad * (180 / FMath::Pi));

	float myXScale = 1 / std::tanf(hFoVRad / 2.0f);
	float myYScale = 1 / std::tanf(vFoVRad * 0.5f);
	float Q = myFarPlane / (myFarPlane - myNearPlane);

	myProjection(1, 1) = myXScale;
	myProjection(2, 2) = myYScale;
	myProjection(3, 3) = Q;
	myProjection(3, 4) = 1.0f / Q;
	myProjection(4, 3) = -Q * myNearPlane;
	myProjection(4, 4) = 0.0f;
}

void Camera::SetTransform(Vector3f aPosition, Vector3f aRotation)
{
	SetPosition(aPosition);
	SetRotation(aRotation);
}

void Camera::SetTransform(Transform someTransform)
{
	myTransform = someTransform;
}

void Camera::SetRotation(Rotator aRotation)
{
	myTransform.SetRotation(aRotation);
}

void Camera::SetPosition(Vector3f aPosition)
{
	myTransform.SetPosition(aPosition);
}
