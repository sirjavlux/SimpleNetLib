#include "NetVector3.h"

NetUtility::NetVector3::NetVector3(const NetVector3& InPosition)
    : x(InPosition.x), y(InPosition.y), z(InPosition.z)
{ }

NetUtility::NetVector3::NetVector3(const float X, const float Y, const float Z) : x(X), y(Y), z(Z)
{ }

float NetUtility::NetVector3::LengthSqr(const NetVector3& InPosition) const
{
    return InPosition.x * x + InPosition.y * y + InPosition.z * z;
}
