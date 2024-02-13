#pragma once

#include <atomic>
#include <memory>

namespace NetUtility
{
struct NetPosition
{
    std::atomic<int> x, y, z;

    NetPosition(const NetPosition& InPosition);
    NetPosition(int X, int Y, int Z);
    NetPosition() = default;
    
    int LengthSqr(const NetPosition& InPosition) const;

    void operator=(const NetPosition& InPosition)
    {
        x = InPosition.x.load();
        y = InPosition.y.load();
        z = InPosition.z.load();
    }
    
    bool operator==(const NetPosition& InPosition) const
    {
        return std::memcmp(this, &InPosition, sizeof(NetPosition)) == 0;
    }

    bool operator!=(const NetPosition& InPosition) const
    {
        return !(*this == InPosition);
    }

    bool operator<(const NetPosition& InPosition) const
    {
        return std::memcmp(this, &InPosition, sizeof(NetPosition)) < 0;
    }
};
}
