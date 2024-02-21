#include "NetVector3.h"

NetUtility::NetVector3::NetVector3(const NetVector3& InPosition)
    : x(InPosition.x.load()), y(InPosition.y.load()), z(InPosition.z.load())
{ }

NetUtility::NetVector3::NetVector3(const int X, const int Y, const int Z) : x(X), y(Y), z(Z)
{ }

int NetUtility::NetVector3::LengthSqr(const NetVector3& InPosition) const
{
    return InPosition.x * x + InPosition.y * y + InPosition.z * z;
}
