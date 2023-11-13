#pragma once
#include <cassert>
#include "Vector3.h"

namespace FMath
{
	constexpr float Pi = 3.14159265358979323846f;
	constexpr float Pi_Half = Pi / 2.f;
	constexpr float DegToRad = Pi / 180.f;
	constexpr float RadToDeg = 180.f / Pi;

	constexpr float SmallNumber = (1.e-8f);
	constexpr float KindaSmallNumber = (1.e-4f);

	constexpr float Tau = Pi * 2.0f;
	constexpr double PiD = 3.1415926535897932384626433832795;
	constexpr double PiHalfD = PiD * 0.5;
	constexpr double TauD = PiD * 2.0;

	template <class T>
	T Lerp(const T& a, const T& b, float t)
	{
		return (a * (1.0f - t)) + (b * t);
	}
	
	template <class T>
	::Tga::Vector3<T> NLerp(const ::Tga::Vector3<T>& a, const ::Tga::Vector3<T>& b, float t)
	{
		return Lerp(a, b, t).GetNormalized();
	}

	template<class T>
	__forceinline T Abs(const T& aValue)
	{
		return aValue < 0 ? aValue * -1 : aValue;
	}

	template <class T>
	__forceinline T Min(const T& aValue0, const T& aValue1)
	{
		return aValue0 < aValue1 ? aValue0 : aValue1;
	}
	template <class T>
	__forceinline T Max(const T& aValue0, const T& aValue1)
	{
		return aValue0 < aValue1 ? aValue1 : aValue0;
	}
	template <class T>
	__forceinline T Clamp(const T& aValue, const T& aMin, const T& aMax)
	{
#ifdef _DEBUG
		assert(aMin <= aMax && "Min value must be equal or less than Max value.");
#endif
		return
			aValue < aMin ? aMin :
			aValue > aMax ? aMax : aValue;
	}

	template <typename T>
	__forceinline T Saturate(const T& aValue)
	{
		return Clamp<T>(aValue, T(0), T(1));
	}

	// Converts a value from a range to another. E.g Value 5 in range 0-10 converted to range 100-200 will become 150.
	template <class T>
	T Remap(T aValue, T aOldMin, T aOldMax, T aNewMin, T aNewMax)
	{
		return (((aValue - aOldMin) * (aNewMax - aNewMin)) / (aOldMax - aOldMin)) + aNewMin;
	}

	// Combines two values into one unique. Warning: for optimization reasons this returns a integer and will overflow if pairing too high numbers.
	// If the function should not make a distinction between (a, b) and (b, a) then sort a and b first.
	int SzudzikPairingFunction(int aValue0, int aValue1);

	// Calculates the linear parameter t that produces the interpolant [aValue] within the range[aFrom, aTo].
	float InverseLerp(const float aFrom, const float aTo, const float aValue);

	template <class T>
	::Tga::Vector3<T> SLerp(::Tga::Vector3<T> start, ::Tga::Vector3<T> end, float percent)
	{
		// Dot product - the cosine of the angle between 2 vectors.
		float dot = start.Dot(end);
		// Clamp it to be in the range of Acos()
		// This may be unnecessary, but floating point
		// precision can be a fickle mistress.
		dot=Clamp(dot, -1.0f, 1.0f);
		// Acos(dot) returns the angle between start and end,
		// And multiplying that by percent returns the angle between
		// start and the final result.
		float theta = acos(dot) * percent;
		::Tga::Vector3<T> RelativeVec = end - start * dot;
		RelativeVec.Normalize();
		// Orthonormal basis
		// The final result.
		::Tga::Vector3<T> returnValue= ((start * cos(theta)) + (RelativeVec * sin(theta)));
		return returnValue;
	}
}