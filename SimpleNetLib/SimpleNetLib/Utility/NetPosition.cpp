#include "NetPosition.h"

NetUtility::NetPosition::NetPosition(const NetPosition& InPosition)
    : x(InPosition.x.load()), y(InPosition.y.load()), z(InPosition.z.load())
{ }

NetUtility::NetPosition::NetPosition(const int X, const int Y, const int Z) : x(X), y(Y), z(Z)
{ }

int NetUtility::NetPosition::LengthSqr(const NetPosition& InPosition) const
{
    return InPosition.x * x + InPosition.y * y + InPosition.z * z;
}
