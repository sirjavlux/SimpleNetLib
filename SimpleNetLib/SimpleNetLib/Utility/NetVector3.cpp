#include "NetVector3.h"

#include <complex>

NetUtility::NetVector3::NetVector3(const NetVector3& InPosition)
    : x(InPosition.x), y(InPosition.y), z(InPosition.z)
{ }

NetUtility::NetVector3::NetVector3(const float X, const float Y, const float Z) : x(X), y(Y), z(Z)
{ }

float NetUtility::NetVector3::DistanceSqr(const NetVector3& InPosition) const
{
    return InPosition.x * x + InPosition.y * y + InPosition.z * z;
}

float NetUtility::NetVector3::LengthSqr() const
{
    const float length = x * x + y * y + z * z;
    return length;
}

float NetUtility::NetVector3::Length() const
{
    return std::sqrt(LengthSqr());
}

void NetUtility::NetVector3::Normalize()
{
    const float length = Length();
    if (length < 0.0001f)
    {
        x = 0.f;
        y = 0.f;
        z = 0.f;
        return;
    }
    
    x /= length;
    y /= length;
    z /= length;
}
