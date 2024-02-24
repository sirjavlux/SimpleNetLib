#pragma once

#include <atomic>
#include <memory>

namespace NetUtility
{
struct NetVector3
{
    float x, y, z;

    NetVector3(const NetVector3& InPosition);
    NetVector3(float X, float Y, float Z);
    NetVector3() = default;
    
    float LengthSqr(const NetVector3& InPosition) const;

    void operator=(const NetVector3& InPosition)
    {
        x = InPosition.x;
        y = InPosition.y;
        z = InPosition.z;
    }
    
    bool operator==(const NetVector3& InPosition) const
    {
        return std::memcmp(this, &InPosition, sizeof(NetVector3)) == 0;
    }

    bool operator!=(const NetVector3& InPosition) const
    {
        return !(*this == InPosition);
    }

    bool operator<(const NetVector3& InPosition) const
    {
        return std::memcmp(this, &InPosition, sizeof(NetVector3)) < 0;
    }
};
}
