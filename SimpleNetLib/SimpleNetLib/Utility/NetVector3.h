#pragma once

#include <atomic>
#include <memory>

namespace NetUtility
{
struct NetVector3
{
    std::atomic<int> x, y, z;

    NetVector3(const NetVector3& InPosition);
    NetVector3(int X, int Y, int Z);
    NetVector3() = default;
    
    int LengthSqr(const NetVector3& InPosition) const;

    void operator=(const NetVector3& InPosition)
    {
        x = InPosition.x.load();
        y = InPosition.y.load();
        z = InPosition.z.load();
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
