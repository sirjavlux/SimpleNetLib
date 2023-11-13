#include "stdafx.h"
#include "Transform.h"

using namespace Tga;

Transform::Transform(Vector3f somePosition, Rotator someRotation, Vector3f someScale) : myPosition(somePosition),
	myRotation(someRotation), myScale(someScale)
{
}

Transform::Transform(Vector3f somePosition, Quaternionf someRotation, Vector3f someScale) : myPosition(somePosition),
	myRotation(someRotation.GetEulerAnglesDegrees()), myScale(someScale)
{
}

void Transform::SetPosition(Vector3f somePosition)
{
	myPosition = somePosition;
}

void Transform::SetRotation(Rotator someRotation)
{
	myRotation = someRotation;
}

void Transform::SetScale(Vector3f someScale)
{
	myScale = someScale;
}

void Transform::AddRotation(Rotator someRotation)
{
	SetRotation(myRotation + someRotation);
}
