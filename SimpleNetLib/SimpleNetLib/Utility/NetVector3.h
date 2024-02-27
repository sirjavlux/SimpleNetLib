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
    
    float DistanceSqr(const NetVector3& InPosition) const;
    float LengthSqr() const;
    float Length() const;

    NetVector3 GetNormalized() const;
    void Normalize();
    
    void operator=(const NetVector3& InPosition)
    {
        x = InPosition.x;
        y = InPosition.y;
        z = InPosition.z;
    }

    void operator*=(const NetVector3& InPosition)
    {
        x *= InPosition.x;
        y *= InPosition.y;
        z *= InPosition.z;
    }

    void operator*=(const float InScalar)
    {
        x *= InScalar;
        y *= InScalar;
        z *= InScalar;
    }

    NetVector3 operator*(const float InScalar) const
    {
        NetVector3 result = *this;
        result.x *= InScalar;
        result.y *= InScalar;
        result.z *= InScalar;

        return result;
    }

    void operator+=(const NetVector3& InPosition)
    {
        x += InPosition.x;
        y += InPosition.y;
        z += InPosition.z;
    }

    void operator-=(const NetVector3& InPosition)
    {
        x -= InPosition.x;
        y -= InPosition.y;
        z -= InPosition.z;
    }

    NetVector3 operator-(const NetVector3& InVector) const
    {
        return { x - InVector.x, y - InVector.y, z - InVector.z };
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
