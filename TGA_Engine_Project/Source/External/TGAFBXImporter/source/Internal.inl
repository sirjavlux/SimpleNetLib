#pragma once
#include <cassert>
#include <string>
#include <xmmintrin.h>

template <class T>
class Matrix33;

template <class T>
class Matrix4x4;

template<class T>
class Vector3;

template<class T>
class Vector2;

typedef Vector3<float> Vector3f;

#include <math.h>
#ifdef _DEBUG
#include <cassert>
#endif // _DEBUG

#pragma warning( push )
#pragma warning( disable : 4201 ) // Nonstandard nameless struct/union.
#ifndef _RETAIL
#include <iostream>
#endif // !_RETAIL

#ifdef max
#undef max
#endif

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
	Vector3<T> NLerp(const Vector3<T>& a, const Vector3<T>& b, float t)
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
	Vector3<T> SLerp(Vector3<T> start, Vector3<T> end, float percent)
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
		Vector3<T> RelativeVec = end - start * dot;
		RelativeVec.Normalize();
		// Orthonormal basis
		// The final result.
		Vector3<T> returnValue= ((start * cos(theta)) + (RelativeVec * sin(theta)));
		return returnValue;
	}
}

	template <class T>
	class Vector2
	{
	public:
		union
		{
			T X;
			T myX;
		};
		union
		{
			T Y;
			T myY;
		};

		Vector2<T>();
		~Vector2<T>() = default;
		Vector2<T>(const T& aX, const T& aY);
		Vector2<T>(const Vector2<T>& aVector) = default;
		Vector2<T>(const T& aScalar);

		template <class U>
		explicit operator Vector2<U>() { return { static_cast<U>(myX), static_cast<U>(myY) }; }

		Vector2<T>& operator=(const Vector2<T>& aVector2) = default;

		template <class OtherType>
		Vector2<T>& operator=(const Vector2<OtherType>& aVector) { X = (T)aVector.X; Y = (T)aVector.Y; return *this; }

		T LengthSqr() const;
		T Length() const;

		Vector2<T> GetNormalized() const;
		Vector2<T>& Normalize();
		Vector2<T> Normal() const;

		T Dot(const Vector2<T>& aVector) const;

		void Set(const T& aX, const T& aY);

		template <class U>
		friend std::ostream& operator<<(std::ostream& os, const Vector2<U>& aVector);
	};

	typedef Vector2<float> Vector2f;
	typedef Vector2<unsigned int>  Vector2ui;

	template <class T> Vector2<T> operator+(const Vector2<T>& aVector0, const Vector2<T>& aVector1);
	template <class T> Vector2<T> operator-(const Vector2<T>& aVector0, const Vector2<T>& aVector1);
	template <class T> Vector2<T> operator*(const Vector2<T>& aVector0, const Vector2<T>& aVector1);
	template <class T> Vector2<T> operator*(const Vector2<T>& aVector, const T& aScalar);
	template <class T> Vector2<T> operator*(const T& aScalar, const Vector2<T>& aVector);
	template <class T> Vector2<T> operator/(const Vector2<T>& aVector, const T& aScalar);
	template <class T> Vector2<T> operator/(const Vector2<T>& aVector, const int& aScalar);
	template <class T> Vector2<T> operator/(const Vector2<T>& aVector, const float& aScalar);
	template <class T> Vector2<T> operator/(const Vector2<T>& aVector0, const Vector2<T>& aVector1);
	template <class T> bool operator==(const Vector2<T>& aVector0, const Vector2<T>& aVector1);
	template <class T> bool operator!=(const Vector2<T>& aVector0, const Vector2<T>& aVector1);

	template <class T> void operator+=(Vector2<T>& aVector0, const Vector2<T>& aVector1);
	template <class T> void operator-=(Vector2<T>& aVector0, const Vector2<T>& aVector1);
	template <class T> void operator*=(Vector2<T>& aVector, const T& aScalar);
	template <class T> void operator/=(Vector2<T>& aVector, const T& aScalar);
	template <class T> std::istream& operator>>(std::istream& in, Vector2<T>& aVec);

#pragma region MemberDefinitions

	template<class T>
	std::istream& operator>>(std::istream& in, Vector2<T>& aVec)
	{
		in >> aVec.X;
		in >> aVec.Y;
		return in;
	}

	template<class T>
	inline Vector2<T>::Vector2() : Vector2(0, 0)
	{}

	template<class T>
	inline Vector2<T>::Vector2(const T& aX, const T& aY) : X(aX), Y(aY)
	{}

	template <class T>
	Vector2<T>::Vector2(const T& aScalar) : X(aScalar), Y(aScalar)
	{}

template<class T>
	inline T Vector2<T>::LengthSqr() const
	{
		return (X * X) + (Y * Y);
	}

	template<class T>
	inline T Vector2<T>::Length() const
	{
		return sqrt((X * X) + (Y * Y));
	}

	template<class T>
	inline Vector2<T> Vector2<T>::GetNormalized() const
	{
		const T magnitude = (X * X) + (Y * Y);

		if (magnitude == 0)
		{
			return *this;
		}

		const T inversedMagnitude = T(1) / sqrt(magnitude);
		return Vector2<T>(X * inversedMagnitude, Y * inversedMagnitude);
	}

	template<class T>
	inline Vector2<T>& Vector2<T>::Normalize()
	{
		const T magnitude = (X * X) + (Y * Y);

		if (magnitude == 0)
		{
			return *this;
		}

		const T inversedMagnitude = T(1) / sqrt(magnitude);
		X *= inversedMagnitude;
		Y *= inversedMagnitude;
		return *this;
	}

	// Returns a copy of the non-normalized normal.
	template<class T> inline Vector2<T> Vector2<T>::Normal() const
	{
		return Vector2<T>(Y, -X);
	}

	template<class T>
	inline T Vector2<T>::Dot(const Vector2<T>& aVector) const
	{
		return (X * aVector.X) + (Y * aVector.Y);
	}

	template<class T>
	inline void Vector2<T>::Set(const T& aX, const T& aY)
	{
		X = aX;
		Y = aY;
	}

#pragma endregion MemberDefinitions

#pragma region OperatorDefinitions
	template <class T>
	Vector2<T> operator+(const Vector2<T>& aVector0, const Vector2<T>& aVector1)
	{
		return Vector2<T>(aVector0.X + aVector1.X, aVector0.Y + aVector1.Y);
	}

	template<class T>
	Vector2<T> operator-(const Vector2<T>& aVector0, const Vector2<T>& aVector1)
	{
		return Vector2<T>(aVector0.X - aVector1.X, aVector0.Y - aVector1.Y);
	}

	template<class T>
	Vector2<T> operator*(const Vector2<T>& aVector0, const Vector2<T>& aVector1)
	{
		return Vector2<T>(aVector0.X * aVector1.X, aVector0.Y * aVector1.Y);
	}

	template <class T>
	Vector2<T> operator*(const Vector2<T>& aVector, const T& aScalar)
	{
		return Vector2<T>(aVector.X * aScalar, aVector.Y * aScalar);
	}

	template <class T>
	Vector2<T> operator*(const T& aScalar, const Vector2<T>& aVector)
	{
		return aVector * aScalar;
	}

	template <class T>
	Vector2<T> operator/(const Vector2<T>& aVector, const T& aScalar)
	{
		return aVector * (1 / aScalar);
	}

	template <class T>
	Vector2<T> operator/(const Vector2<T>& aVector, const int& aScalar)
	{
		return { aVector.X / static_cast<T>(aScalar), aVector.Y / static_cast<T>(aScalar) };
	}

	template <class T>
	Vector2<T> operator/(const Vector2<T>& aVector, const float& aScalar)
	{
		return { aVector.X / static_cast<T>(aScalar), aVector.Y / static_cast<T>(aScalar) };
	}

template<class T>
	Vector2<T> operator/(const Vector2<T>& aVector0, const Vector2<T>& aVector1)
	{
		return Vector2<T>{ aVector0.X / aVector1.X, aVector0.Y / aVector1.Y };
	}

	template<class T>
	bool operator==(const Vector2<T>& aVector0, const Vector2<T>& aVector1)
	{
		return aVector0.X == aVector1.X && aVector0.Y == aVector1.Y;
	}

	template <class T>
	bool operator!=(const Vector2<T>& aVector0, const Vector2<T>& aVector1)
	{
		return !(aVector0 == aVector1);
	}

template <class T>
	void operator+=(Vector2<T>& aVector0, const Vector2<T>& aVector1)
	{
		aVector0.X += aVector1.X;
		aVector0.Y += aVector1.Y;
	}

	template <class T>
	void operator-=(Vector2<T>& aVector0, const Vector2<T>& aVector1)
	{
		aVector0.X -= aVector1.X;
		aVector0.Y -= aVector1.Y;
	}

	template <class T>
	void operator*=(Vector2<T>& aVector, const T& aScalar)
	{
		aVector.X *= aScalar;
		aVector.Y *= aScalar;
	}

	template <class T>
	void operator/=(Vector2<T>& aVector, const T& aScalar)
	{
		const T inv = (1 / aScalar);
		aVector.X *= inv;
		aVector.Y *= inv;
	}
	template <class T>
	std::ostream& operator<<(std::ostream& os, const Vector2<T>& aVector)
	{
		return os << "{ X: " << aVector.X << " Y: " << aVector.Y << " }";
	}
#pragma endregion OperatorDefinitions


	template <class T>
	class Quaternion
	{
	public:
		union
		{
			T myValues[4];
			//JA JAG VET!!! Men typ de flesta st�llen �r W f�rst!!!
			struct { T W; T X; T Y; T Z; };
		};

		Quaternion<T>();
		Quaternion<T>(const T& aW, const T& aX, const T& aY, const T& aZ);
		Quaternion<T>(const T& aYaw, const T& aPitch, const T& aRoll);
		Quaternion<T>(const Vector3<T>& aYawPitchRoll);
		Quaternion<T>(const Vector3<T>& aVector, const T aAngle);
		Quaternion<T>(const Matrix4x4<T>& aMatrix);

		//Set from Unity values - Flip X and Z values, W and Y remains the same
		void SetFromUnityValues(const T aW, const T aX, const T aY, const T aZ);

		void RotateWithEuler(const Vector3<T>& anEuler);

		inline void Normalize();
		inline Quaternion<T> GetNormalized() const;
		inline Quaternion<T> GetConjugate() const;

		Quaternion<T>& operator=(const Quaternion<T>& aQuat) = default;

		T Length() const;
		T Length2() const;
		inline Vector3<T> GetEulerAnglesRadians() const;
		inline Vector3<T> GetEulerAnglesRadiansd() const;
		inline Vector3<T> GetEulerAnglesDegrees() const;
		inline Matrix33<T> GetRotationMatrix33() const;
		inline Matrix4x4<T> GetRotationMatrix44() const;
		inline T Dot(const Quaternion<T>& aQuat) const;

		inline Vector3<T> GetRight() const;
		inline Vector3<T> GetUp() const;
		inline Vector3<T> GetForward() const;
		// Rotates a vector by the rotation stored in the Quaternion.
		inline static Vector3<T> RotateVectorByQuaternion(const Quaternion<T>& aQuaternion, const Vector3f& aVectorToRotate);
		inline static Quaternion<T> Lerp(const Quaternion<T>& aQuatA, const Quaternion<T>& aQuatB, const T& aDelta);
		inline static Quaternion<T> Slerp(const Quaternion<T>& aQuatA, const Quaternion<T>& aQuatB, const T& aDelta);
	};

	template<class T>
	inline Quaternion<T>::Quaternion()
	{
		W = static_cast<T>(1);
		X = static_cast<T>(0);
		Y = static_cast<T>(0);
		Z = static_cast<T>(0);
	}

	template<class T>
	inline Quaternion<T>::Quaternion(const T& aW, const T& aX, const T& aY, const T& aZ)
	{
		W = aW;
		X = aX;
		Y = aY;
		Z = aZ;
	}

	template<class T>
	inline Quaternion<T>::Quaternion(const T& aYaw, const T& aPitch, const T& aRoll)
	{
		T cy = cos(aYaw * T(0.5));
		T sy = sin(aYaw * T(0.5));
		T cr = cos(aRoll * T(0.5));
		T sr = sin(aRoll * T(0.5));
		T cp = cos(aPitch * T(0.5));
		T sp = sin(aPitch * T(0.5));

		W = cy * cr * cp + sy * sr * sp;
		X = cy * sr * cp - sy * cr * sp;
		Y = cy * cr * sp + sy * sr * cp;
		Z = sy * cr * cp - cy * sr * sp;
	}

	template<class T>
	inline Quaternion<T>::Quaternion(const Vector3<T>& aYawPitchRoll)
	{
		T cx = cos(aYawPitchRoll.X * T(0.5));
		T cy = cos(aYawPitchRoll.Y * T(0.5));
		T cz = cos(aYawPitchRoll.Z * T(0.5));
		T sx = sin(aYawPitchRoll.X * T(0.5));
		T sy = sin(aYawPitchRoll.Y * T(0.5));
		T sz = sin(aYawPitchRoll.Z * T(0.5));

		W = cx * cy * cz + sx * sy * sz;
		X = sx * cy * cz - cx * sy * sz;
		Y = cx * sy * cz + sx * cy * sz;
		Z = cx * cy * sz - sx * sy * cz;

	}

	template<class T>
	inline Quaternion<T>::Quaternion(const Vector3<T>& aVector, const T aAngle)
	{
		T halfAngle = aAngle / T(2);
		W = cos(halfAngle);
		T halfAngleSin = sin(halfAngle);
		X = aVector.X * halfAngleSin;
		Y = aVector.Y * halfAngleSin;
		Z = aVector.Z * halfAngleSin;

		//W = cos(aAngle / T(2));
		//X = cos(axis.X) * sin(aAngle / T(2));
		//Y = cos(axis.Y) * sin(aAngle / T(2));
		//Z = cos(axis.Z) * sin(aAngle / T(2));
	}

	template<class T>
	inline Quaternion<T>::Quaternion(const Matrix4x4<T>& aMatrix)
	{
		W = std::sqrt(std::max(T(0), T(1) + aMatrix.m11 + aMatrix.m22 + aMatrix.m33)) * T(0.5);
		X = std::sqrt(std::max(T(0), T(1) + aMatrix.m11 - aMatrix.m22 - aMatrix.m33)) * T(0.5);
		Y = std::sqrt(std::max(T(0), T(1) - aMatrix.m11 + aMatrix.m22 - aMatrix.m33)) * T(0.5);
		Z = std::sqrt(std::max(T(0), T(1) - aMatrix.m11 - aMatrix.m22 + aMatrix.m33)) * T(0.5);
		X = std::copysign(X, aMatrix.m32 - aMatrix.m23);
		Y = std::copysign(Y, aMatrix.m13 - aMatrix.m31);
		Z = std::copysign(Z, aMatrix.m21 - aMatrix.m12);


		//T fourXSquaredMinus1 = aMatrix.myRows[0][0] - aMatrix.myRows[1][1] - aMatrix.myRows[2][2];
		//T fourYSquaredMinus1 = aMatrix.myRows[1][1] - aMatrix.myRows[0][0] - aMatrix.myRows[2][2];
		//T fourZSquaredMinus1 = aMatrix.myRows[2][2] - aMatrix.myRows[0][0] - aMatrix.myRows[1][1];
		//T fourWSquaredMinus1 = aMatrix.myRows[0][0] + aMatrix.myRows[1][1] + aMatrix.myRows[2][2];
		//
		//int biggestIndex = 0;
		//T fourBiggestSquaredMinus1 = fourWSquaredMinus1;
		//if (fourXSquaredMinus1 > fourBiggestSquaredMinus1)
		//{
		//	fourBiggestSquaredMinus1 = fourXSquaredMinus1;
		//	biggestIndex = 1;
		//}
		//if (fourYSquaredMinus1 > fourBiggestSquaredMinus1)
		//{
		//	fourBiggestSquaredMinus1 = fourYSquaredMinus1;
		//	biggestIndex = 2;
		//}
		//if (fourZSquaredMinus1 > fourBiggestSquaredMinus1)
		//{
		//	fourBiggestSquaredMinus1 = fourZSquaredMinus1;
		//	biggestIndex = 3;
		//}
		//
		//T biggestVal = sqrt(fourBiggestSquaredMinus1 + static_cast<T>(1)) * static_cast<T>(0.5);
		//T mult = static_cast<T>(0.25) / biggestVal;
		//
		//switch (biggestIndex)
		//{
		//case 0:
		//	W = biggestVal;
		//	X = (aMatrix.myRows[1][2] - aMatrix.myRows[2][1]) * mult;
		//	Y = (aMatrix.myRows[2][0] - aMatrix.myRows[0][2]) * mult;
		//	Z = (aMatrix.myRows[0][1] - aMatrix.myRows[1][0]) * mult;
		//	break;
		//case 1:
		//	W = (aMatrix.myRows[1][2] - aMatrix.myRows[2][1]) * mult;
		//	X = biggestVal;
		//	Y = (aMatrix.myRows[0][1] + aMatrix.myRows[1][0]) * mult;
		//	Z = (aMatrix.myRows[2][0] + aMatrix.myRows[0][2]) * mult;
		//	break;
		//case 2:
		//	W = (aMatrix.myRows[2][0] - aMatrix.myRows[0][2]) * mult;
		//	X = (aMatrix.myRows[0][1] + aMatrix.myRows[1][0]) * mult;
		//	Y = biggestVal;
		//	Z = (aMatrix.myRows[1][2] + aMatrix.myRows[2][1]) * mult;
		//	break;
		//case 3:
		//	W = (aMatrix.myRows[0][1] - aMatrix.myRows[1][0]) * mult;
		//	X = (aMatrix.myRows[2][0] + aMatrix.myRows[0][2]) * mult;
		//	Y = (aMatrix.myRows[1][2] + aMatrix.myRows[2][1]) * mult;
		//	Z = biggestVal;
		//	break;
		//}
	}

#ifndef _RETAIL
	// Prints the Quaternion, example use case: std::cout << q << std::endl;
	template <class T> inline std::ostream& operator<<(std::ostream& stream, const Quaternion<T>& aQuat)
	{
		std::cout << "W: " << aQuat.W << "  X: " << aQuat.X << "  Y: " << aQuat.Y << "  Z: " << aQuat.Z;
		return stream;
	}
#endif //!_RETAIL

	template <class T> inline Quaternion<T> operator*(const Quaternion<T>& aQuat, const T& aScalar)
	{
		return Quaternion<T>(aQuat.W * aScalar, aQuat.X * aScalar, aQuat.Y * aScalar, aQuat.Z * aScalar);
	}

	template <class T> inline Quaternion<T> operator*(const T& aScalar, const Quaternion<T>& aQuat)
	{
		return Quaternion<T>(aQuat.W * aScalar, aQuat.X * aScalar, aQuat.Y * aScalar, aQuat.Z * aScalar);
	}

	template <class T> inline Quaternion<T> operator*(const Quaternion<T>& aQuat0, const Quaternion<T>& aQuat1)
	{
		return Quaternion<T>(
			(aQuat1.W * aQuat0.W) - (aQuat1.X * aQuat0.X) - (aQuat1.Y * aQuat0.Y) - (aQuat1.Z * aQuat0.Z),
			(aQuat1.W * aQuat0.X) + (aQuat1.X * aQuat0.W) + (aQuat1.Y * aQuat0.Z) - (aQuat1.Z * aQuat0.Y),
			(aQuat1.W * aQuat0.Y) + (aQuat1.Y * aQuat0.W) + (aQuat1.Z * aQuat0.X) - (aQuat1.X * aQuat0.Z),
			(aQuat1.W * aQuat0.Z) + (aQuat1.Z * aQuat0.W) + (aQuat1.X * aQuat0.Y) - (aQuat1.Y * aQuat0.X)
			);
	}

	template <class T> inline void operator*=(Quaternion<T>& aQuat, const T& aScalar)
	{
		aQuat.W *= aScalar;
		aQuat.X *= aScalar;
		aQuat.Y *= aScalar;
		aQuat.Z *= aScalar;
	}

	template <class T> inline void operator*=(Quaternion<T>& aQuat0, const Quaternion<T>& aQuat1)
	{
		T w = aQuat0.W;
		T x = aQuat0.X;
		T y = aQuat0.Y;
		T z = aQuat0.Z;

		//aQuat0.W = W * aQuat1.W - X * aQuat1.X - Y * aQuat1.Y - Z * aQuat1.Z;
		//aQuat0.X = W * aQuat1.X + X * aQuat1.W + Y * aQuat1.Z - Z * aQuat1.Y;
		//aQuat0.Y = W * aQuat1.Y - X * aQuat1.Z + Y * aQuat1.W + Z * aQuat1.X;
		//aQuat0.Z = W * aQuat1.Z + X * aQuat1.Y - Y * aQuat1.X + Z * aQuat1.W;


		aQuat0.W = (aQuat1.W * w) - (aQuat1.X * x) - (aQuat1.Y * y) - (aQuat1.Z * z);
		aQuat0.X = (aQuat1.W * x) + (aQuat1.X * w) + (aQuat1.Y * z) - (aQuat1.Z * y);
		aQuat0.Y = (aQuat1.W * y) + (aQuat1.Y * w) + (aQuat1.Z * x) - (aQuat1.X * z);
		aQuat0.Z = (aQuat1.W * z) + (aQuat1.Z * w) + (aQuat1.X * y) - (aQuat1.Y * x);

	}

	template <class T> inline Quaternion<T> operator/(const Quaternion<T>& aQuat, const T& aScalar)
	{
		return Quaternion<T>(aQuat.W / aScalar, aQuat.X / aScalar, aQuat.Y / aScalar, aQuat.Z / aScalar);
	}

	template <class T> inline Quaternion<T> operator-(const Quaternion<T>& aQuatA, const Quaternion<T>& aQuatB)
	{
		return Quaternion<T>(aQuatA.W - aQuatB.W, aQuatA.X - aQuatB.X, aQuatA.Y - aQuatB.Y, aQuatA.Z - aQuatB.Z);
	}

	template <class T> inline Quaternion<T> operator-(const Quaternion<T>& aQuat)
	{
		return Quaternion<T>(-aQuat.W, -aQuat.X, -aQuat.Y, -aQuat.Z);
	}

	template <class T> inline Quaternion<T> operator+(const Quaternion<T>& aQuatA, const Quaternion<T>& aQuatB)
	{
		return Quaternion<T>(aQuatA.W + aQuatB.W, aQuatA.X + aQuatB.X, aQuatA.Y + aQuatB.Y, aQuatA.Z + aQuatB.Z);
	}

	template <class T> inline void operator+=(Quaternion<T>& aQuatA, const Quaternion<T>& aQuatB)
	{
		aQuatA.W += aQuatB.W;
		aQuatA.X += aQuatB.X;
		aQuatA.Y += aQuatB.Y;
		aQuatA.Z += aQuatB.Z;
	}

	template<class T>
	inline void Quaternion<T>::SetFromUnityValues(const T aW, const T aX, const T aY, const T aZ)
	{
		W = aW;
		X = -aX;
		Y = aY;
		Z = -aZ;
	}

	template<class T>
	inline void Quaternion<T>::RotateWithEuler(const Vector3<T>& anEuler)
	{
		*this *= Quaternion(anEuler);
	}

	template<class T>
	inline void Quaternion<T>::Normalize()
	{
		T length = T(1) / Length();
		W *= length;
		X *= length;
		Y *= length;
		Z *= length;
	}

	template<class T>
	inline Quaternion<T> Quaternion<T>::GetNormalized() const
	{
		T length = T(1) / Length();
		return Quaternion<T>(W * length, X * length, Y * length, Z * length);
	}

	template<class T>
	inline Quaternion<T> Quaternion<T>::GetConjugate() const
	{
		return Quaternion<T>(W, -X, -Y, -Z);
	}

	template<class T>
	inline T Quaternion<T>::Length2() const
	{
		return (X * X) + (Y * Y) + (Z * Z) + (W * W);
	}

	template<class T>
	inline T Quaternion<T>::Length() const
	{
		return sqrt(Length2());
	}

	template<class T>
	inline Matrix33<T> Quaternion<T>::GetRotationMatrix33() const
	{
		Matrix33<T> result;
		T qxx(X * X);
		T qyy(Y * Y);
		T qzz(Z * Z);
		T qxz(X * Z);
		T qxy(X * Y);
		T qyz(Y * Z);
		T qwx(W * X);
		T qwy(W * Y);
		T qwz(W * Z);

		result.m11 = T(1) - T(2) * (qyy + qzz);
		result.m21 = T(2) * (qxy + qwz);
		result.m31 = T(2) * (qxz - qwy);

		result.m12 = T(2) * (qxy - qwz);
		result.m22 = T(1) - T(2) * (qxx + qzz);
		result.m32 = T(2) * (qyz + qwx);

		result.m13 = T(2) * (qxz + qwy);
		result.m23 = T(2) * (qyz - qwx);
		result.m33 = T(1) - T(2) * (qxx + qyy);
		return result;
	}

	template<class T>
	inline Matrix4x4<T> Quaternion<T>::GetRotationMatrix44() const
	{
		Matrix4x4<T> result;
		T qxx(X * X);
		T qyy(Y * Y);
		T qzz(Z * Z);

		T qxz(X * Z);
		T qxy(X * Y);
		T qyz(Y * Z);

		T qwx(W * X);
		T qwy(W * Y);
		T qwz(W * Z);

	
		result.m11 = T(1) - T(2) * (qyy + qzz);
		result.m21 = T(2) * (qxy + qwz);
		result.m31 = T(2) * (qxz - qwy);
		
		result.m12 = T(2) * (qxy - qwz);
		result.m22 = T(1) - T(2) * (qxx + qzz);
		result.m32 = T(2) * (qyz + qwx);

		result.m13 = T(2) * (qxz + qwy);
		result.m23 = T(2) * (qyz - qwx);
		result.m33 = T(1) - T(2) * (qxx + qyy);
		return result;
	}

	template<class T>
	inline T Quaternion<T>::Dot(const Quaternion<T>& aQuat) const
	{
		return X * aQuat.X + Y * aQuat.Y + Z * aQuat.Z + W * aQuat.W;
	}

	template<class T>
	inline Vector3<T> Quaternion<T>::GetRight() const
	{
		Vector3<T> right = { 1.f, 0.f, 0.f };
		return Quaternion<T>::RotateVectorByQuaternion(*this, right);;
	}

	template<class T>
	inline Vector3<T> Quaternion<T>::GetUp() const
	{
		Vector3<T> up = { 0.f, 1.f, 0.f };
		return Quaternion<T>::RotateVectorByQuaternion(*this, up);
	}

	template<class T>
	inline Vector3<T> Quaternion<T>::GetForward() const
	{
		Vector3<T> forward = { 0.f, 0.f, 1.f };
		return Quaternion<T>::RotateVectorByQuaternion(*this, forward);
	}

	template<class T>
	inline Vector3<T> Quaternion<T>::GetEulerAnglesDegrees() const
	{
		return GetEulerAnglesRadians() * FMath::RadToDeg;
	}


	template<class T>
	inline Vector3<T> Quaternion<T>::GetEulerAnglesRadians() const
	{
		// roll (Z-axis rotation)
		T sinr = T(2.0) * (W * X + Y * Z);
		T cosr = T(1.0) - T(2.0) * (X * X + Y * Y);
		T pitch = atan2(sinr, cosr);// roll

		// pitch (X-axis rotation)
		T sinp = T(2.0) * (W * Y - Z * X);
		T yaw = T(0.0); // pitch
		if (abs(sinp) >= T(1))
		{
			yaw = copysign(FMath::Pi * T(0.5), sinp); // use 90 degrees if out of range //pitch
		}
		else
		{
			yaw = asin(sinp); // pitch
		}

		// yaw (Y-axis rotation)
		T siny = T(2.0) * (W * Z + X * Y);
		T cosy = T(1.0) - T(2.0) * (Y * Y + Z * Z);
		T roll = atan2(siny, cosy); // yaw

		return Vector3<T>(pitch, yaw, roll);
		//return Vector3<T>(roll, pitch, yaw);
	}

	template<class T>
	inline Vector3<T> Quaternion<T>::GetEulerAnglesRadiansd() const
	{
		float yaw, pitch, roll;
		float test = X * Y + Z * W;
		if (test > 0.499) { // singularity at north pole
			yaw = 2 * atan2(X, W);
			roll = FMath::Pi_Half;
			pitch = 0;
			return Vector3<T>{ pitch, yaw, roll };
		}
		if (test < -0.499) { // singularity at south pole
			yaw = -2 * atan2(X, W);
			roll = -FMath::Pi_Half;
			pitch = 0;
			return Vector3<T>{ pitch, yaw, roll };
		}
		float sqx = X * X;
		float sqy = Y * Y;
		float sqz = Z * Z;
		yaw = atan2(2 * Y * W - 2 * X * Z, 1 - 2 * sqy - 2 * sqz);
		roll = asin(2 * test);
		pitch = atan2(2 * X * W - 2 * Y * Z, 1 - 2 * sqx - 2 * sqz);
		return Vector3<T>{ pitch, yaw, roll };
	}



	template<class T>
	inline Vector3<T> Quaternion<T>::RotateVectorByQuaternion(const Quaternion<T>& aQuaternion, const Vector3f& aVectorToRotate)
	{
		Vector3<T> v(aQuaternion.X, aQuaternion.Y, aQuaternion.Z);
		Vector3<T> result =
			2.0f * v.Dot(aVectorToRotate) * v
			+ (aQuaternion.W * aQuaternion.W - v.Dot(v)) * aVectorToRotate
			+ 2.0f * aQuaternion.W * v.Cross(aVectorToRotate);
		return result;
	}

	template<class T>
	inline Quaternion<T> Quaternion<T>::Lerp(const Quaternion<T>& aQuatA, const Quaternion<T>& aQuatB, const T& aDelta)
	{
		Quaternion<T> Result;
		float deltaInv = 1 - aDelta;
		Result.W = deltaInv * aQuatA.W + aDelta * aQuatB.W;
		Result.X = deltaInv * aQuatA.X + aDelta * aQuatB.X;
		Result.Y = deltaInv * aQuatA.Y + aDelta * aQuatB.Y;
		Result.Z = deltaInv * aQuatA.Z + aDelta * aQuatB.Z;
		Result.Normalize();
		return Result;
	}

	template<class T>
	inline Quaternion<T> Quaternion<T>::Slerp(const Quaternion<T>& aQuatA, const Quaternion<T>& aQuatB, const T& aDelta)
	{
		Quaternion<T> qz = aQuatB;

		T cosTheta = aQuatA.Dot(aQuatB);

		// If cosTheta < 0, the interpolation will take the long way around the sphere. 
		// To fix this, one quat must be negated.
		if (cosTheta < T(0))
		{
			qz = -qz;
			cosTheta = -cosTheta;
		}

		const T dotThreshold = static_cast<T>(0.9995);
		// Perform a linear interpolation when cosTheta is close to 1 to avoid side effect of sin(angle) becoming a zero denominator
		if (cosTheta > T(1) - dotThreshold)
		{
			// Linear interpolation
			return Lerp(aQuatA, qz, aDelta);
		}
		else
		{
			// Essential Mathematics, page 467
			T angle = acos(cosTheta);
			return (sin((T(1) - aDelta) * angle) * aQuatA + sin(aDelta * angle) * qz) / sin(angle);
		}

	}

	typedef Quaternion<float> Quaternionf;
	typedef Quaternion<double> Quaterniond;
	typedef Quaternion<int> Quaternioni;
	typedef Quaternion<unsigned int> Quaternionui;
	typedef Quaternion<short>Quaternions;
	typedef Quaternion<unsigned short> Quaternionus;
	typedef Quaternion<char> Quaternionc;
	typedef Quaternion<unsigned char> Quaternionuc;

	typedef Quaternion<float> Quatf;
	typedef Quaternion<double> Quatd;
	typedef Quaternion<int> Quati;
	typedef Quaternion<unsigned int> Quatui;
	typedef Quaternion<short>Quats;
	typedef Quaternion<unsigned short> Quatus;
	typedef Quaternion<char> Quatc;
	typedef Quaternion<unsigned char> Quatuc;

#pragma warning( pop )


#pragma warning (disable : 4201) // Nonstandard nameless struct/union.

	template <class T>
	class Vector4
	{
	public:
		union
		{
			T myValues[4];
			struct { T X; T Y; T Z; T W; };
			struct { Vector3<T>xyz; T w; };
			struct { Vector2<T>xy; Vector2<T> zw; };
		};

		Vector4<T>();
		Vector4<T>(const T& aX, const T& aY, const T& aZ, const T& aW);
		Vector4<T>(const Vector4<T>& aVector4) = default;
		Vector4<T>(const Vector3<T>& aVector3);
		Vector4<T>(const Vector3<T>& aVector3, const T& aW);
		Vector4<T>(const Vector2<T>& aXY, const Vector2<T>& aZW);
		Vector4<T>(const Vector2<T>& aXY, const T& aZ, const T& aW);
		Vector4<T>& operator=(const Vector4<T>& aVector4) = default;
		Vector4<T>& operator=(const Vector3<T>& aVector3);
		bool operator==(const Vector4<T>& aVector4) const;
		bool operator!=(const Vector4<T>& aVector4) const;
		~Vector4<T>() = default;

		T LengthSqr() const;
		T Length() const;

		Vector4<T> GetNormalized() const;
		void Normalize();

		T Dot(const Vector4<T>& aVector) const;
		inline static Vector4<T> Slerp(const Vector4<T>& aQuatA, const Vector4<T>& aQuatB, const T& aDelta);

		Vector3<T> ToVector3();
	};

	typedef Vector4<float> Vector4f;
	template <class T> Vector4<T> operator+(const Vector4<T>& aVector0, const Vector4<T>& aVector1);
	template <class T> Vector4<T> operator-(const Vector4<T>& aVector0, const Vector4<T>& aVector1);
	template <class T> Vector4<T> operator*(const Vector4<T>& aVector, const T& aScalar);
	template <class T> Vector4<T> operator*(const T& aScalar, const Vector4<T>& aVector);
	template <class T> Vector4<T> operator/(const Vector4<T>& aVector, const T& aScalar);
	template <class T> void operator+=(Vector4<T>& aVector0, const Vector4<T>& aVector1);
	template <class T> void operator-=(Vector4<T>& aVector0, const Vector4<T>& aVector1);
	template <class T> void operator*=(Vector4<T>& aVector, const T& aScalar);
	template <class T> void operator/=(Vector4<T>& aVector, const T& aScalar);
	template <class T> std::istream& operator>>(std::istream& in, Vector4<T>& aVec);
	template <class T> std::ostream& operator<<(std::ostream& out, Vector4<T>& aVec);
#pragma region MemberDefinitions

	template <class T>
	std::istream& operator>>(std::istream& in, Vector4<T>& aVec)
	{
		in >> aVec.X;
		in >> aVec.Y;
		in >> aVec.Z;
		in >> aVec.W;
		return in;
	}

template <class T>
	std::ostream& operator<<(std::ostream& out, const Vector4<T>& aVec)
	{
		out << aVec.X << " ";
		out << aVec.Y << " ";
		out << aVec.Z << " ";
		out << aVec.W << " ";
		return out;
	}

	template <class T>
	Vector3<T> Vector4<T>::ToVector3()
	{
		return Vector3<T>(X, Y, Z);
	}

	template<class T>
	inline Vector4<T>::Vector4() : Vector4(0, 0, 0, 0)
	{
	}

	template<class T>
	inline Vector4<T>::Vector4(const T& aX, const T& aY, const T& aZ, const T& aW) : X(aX), Y(aY), Z(aZ), W(aW)
	{
	}

	template<class T>
	inline Vector4<T>::Vector4(const Vector3<T>& aVector3)
	{
		X = aVector3.X;
		Y = aVector3.Y;
		Z = aVector3.Z;
		W = static_cast<T>(1);
	}

	template<class T>
	inline Vector4<T>::Vector4(const Vector3<T>& aVector3, const T& aW)
	{
		xyz = aVector3;
		W = aW;
	}

	template<class T>
	inline Vector4<T>::Vector4(const Vector2<T>& aXY, const Vector2<T>& aZW)
	{
		xy = aXY;
		zw = aZW;
	}

	template<class T>
	inline Vector4<T>::Vector4(const Vector2<T>& aXY, const T& aZ, const T& aW)
	{
		xy = aXY;
		Z = aZ;
		W = aW;
	}

	template<class T>
	inline Vector4<T>& Vector4<T>::operator=(const Vector3<T>& aVector3)
	{
		X = aVector3.X;
		Y = aVector3.Y;
		Z = aVector3.Z;
		return *this;
	}

	template<class T>
	inline bool Vector4<T>::operator==(const Vector4<T>& aVector4) const
	{
		return X == aVector4.X && Y == aVector4.Y && Z == aVector4.Z && W == aVector4.W;
	}

	template <class T>
	bool Vector4<T>::operator!=(const Vector4<T>& aVector4) const
	{
		return !(*this == aVector4);
	}

template<class T>
	inline T Vector4<T>::LengthSqr() const
	{
		return (X * X) + (Y * Y) + (Z * Z) + (W * W);
	}

	template<class T>
	inline T Vector4<T>::Length() const
	{
		return sqrt((X * X) + (Y * Y) + (Z * Z) + (W * W));
	}

	template<class T>
	inline Vector4<T> Vector4<T>::GetNormalized() const
	{
		const T magnitude = (X * X) + (Y * Y) + (Z * Z) + (W * W);

#ifdef _DEBUG
		assert(magnitude != 0 && "Tried to normalize a null vector ");
#endif // _DEBUG

		const T inversedMagnitude = T(1) / sqrt(magnitude);
		return Vector4<T>(X * inversedMagnitude, Y * inversedMagnitude, Z * inversedMagnitude, W * inversedMagnitude);
	}

	template<class T>
	inline void Vector4<T>::Normalize()
	{
		const T magnitude = (X * X) + (Y * Y) + (Z * Z) + (W * W);

#ifdef _DEBUG
		//assert(magnitude != 0 && "Tried to normalize a null vector ");
#endif // _DEBUG

		const T inversedMagnitude = T(1) / sqrt(magnitude);
		X = X * inversedMagnitude;
		Y = Y * inversedMagnitude;
		Z = Z * inversedMagnitude;
		W = W * inversedMagnitude;
	}
	template<class T>
	inline T Vector4<T>::Dot(const Vector4<T>& aVector) const
	{
		return (X * aVector.X) + (Y * aVector.Y) + (Z * aVector.Z) + (W * aVector.W);
	}

#pragma endregion MemberDefinitions

#pragma region OperatorDefinitions

	template <class T>
	Vector4<T> operator+(const Vector4<T>& aVector0, const Vector4<T>& aVector1)
	{
		return Vector4<T>(aVector0.X + aVector1.X, aVector0.Y + aVector1.Y, aVector0.Z + aVector1.Z, aVector0.W + aVector1.W);
	}

	template<class T>
	Vector4<T> operator-(const Vector4<T>& aVector0, const Vector4<T>& aVector1)
	{
		return Vector4<T>(aVector0.X - aVector1.X, aVector0.Y - aVector1.Y, aVector0.Z - aVector1.Z, aVector0.W - aVector1.W);
	}

	template <class T>
	Vector4<T> operator*(const Vector4<T>& aVector, const T& aScalar)
	{
		return Vector4<T>(aVector.X * aScalar, aVector.Y * aScalar, aVector.Z * aScalar, aVector.W * aScalar);
	}

	template <class T>
	Vector4<T> operator*(const T& aScalar, const Vector4<T>& aVector)
	{
		return aVector * aScalar;
	}

	template <class T>
	Vector4<T> operator/(const Vector4<T>& aVector, const T& aScalar)
	{
		return aVector * (1 / aScalar);
	}

	template <class T>
	void operator+=(Vector4<T>& aVector0, const Vector4<T>& aVector1)
	{
		aVector0.X += aVector1.X;
		aVector0.Y += aVector1.Y;
		aVector0.Z += aVector1.Z;
		aVector0.W += aVector1.W;
	}

	template <class T>
	void operator-=(Vector4<T>& aVector0, const Vector4<T>& aVector1)
	{
		aVector0.X -= aVector1.X;
		aVector0.Y -= aVector1.Y;
		aVector0.Z -= aVector1.Z;
		aVector0.W -= aVector1.W;
	}

	template <class T>
	void operator*=(Vector4<T>& aVector, const T& aScalar)
	{
		aVector.X *= aScalar;
		aVector.Y *= aScalar;
		aVector.Z *= aScalar;
		aVector.W *= aScalar;
	}

	template <class T>
	void operator/=(Vector4<T>& aVector, const T& aScalar)
	{
		const T inv = (1 / aScalar);
		aVector.X *= inv;
		aVector.Y *= inv;
		aVector.Z *= inv;
		aVector.W *= inv;
	}

#pragma endregion OperatorDefinitions
#pragma region StaticFunctions

	template<class T>
	inline Vector4<T> Vector4<T>::Slerp(const Vector4<T>& aQuatA, const Vector4<T>& aQuatB, const T& aDelta)
	{
		Vector4<T> qz = aQuatB;

		T cosTheta = aQuatA.Dot(aQuatB);

		// If cosTheta < 0, the interpolation will take the long way around the sphere.
		// To fix this, one quat must be negated.
		if (cosTheta < T(0))
		{
			qz = { -qz.X, -qz.Y,-qz.Z, -qz.W };
			cosTheta = -cosTheta;
		}

		const T dotThreshold = static_cast<T>(0.9995);
		// Perform a linear interpolation when cosTheta is close to 1 to avoid side effect of sin(angle) becoming a zero denominator
		if (cosTheta > T(1) - dotThreshold)
		{
			// Linear interpolation
			return aQuatA + (qz - aQuatA) * aDelta;
		}
		else
		{
			// Essential Mathematics, page 467
			T angle = acos(cosTheta);
			return (sin((T(1) - aDelta) * angle) * aQuatA + sin(aDelta * angle) * qz) / sin(angle);
		}
	}
#pragma endregion

#pragma warning (disable : 4201) // Nonstandard nameless struct/union.
	template <class T>
	class Vector4;

	template <class T>
	class Vector2;

	template <class T>
	class Vector3
	{
	public:
		union
		{
			T myValues[3];
			struct { T X; T Y; T Z; };
			struct { T Roll; T Pitch; T Yaw; };
		};

		static const Vector3<T> Zero;
		static const Vector3<T> One;
		static const Vector3<T> Up;
		static const Vector3<T> Forward;
		static const Vector3<T> Right;

		Vector3<T>( );
		Vector3<T>(const T& aX, const T& aY, const T& aZ);
		Vector3<T>(const T& aScalar);
		Vector3<T>(const Vector3<T>& aVector3) = default;
		Vector3<T>(Vector3<T>&& aVector3) = default;
		Vector3<T>& operator=(const Vector3<T>& aVector3) = default;
		Vector3<T>& operator=(Vector3<T>&& aVector3) = default;
		//Creates a vector (aX, aY, aZ)
		Vector3<T>(const Vector4<T>& aVector4);
		Vector3<T>(const Vector2<T>& aXY, const T& aZ);

		T LengthSqr( ) const;
		T Length( ) const;

		Vector3<T> GetNormalized( ) const;
		void Normalize( );

		T Dot(const Vector3<T>& aVector) const;
		Vector3<T> Cross(const Vector3<T>& aVector) const;

		static Vector3<T> Abs(const Vector3<T>& aVector);
		static T Distance(const Vector3<T>& aVector0, const Vector3<T>& aVector1);

		static Vector3<T> Lerp(const Vector3<T>& aStart, const Vector3<T>& aEnd, const float aPercent);

		static Vector3<T> NLerp(const Vector3<T>& aStart, const Vector3<T>& aEnd, const float aPercent);

		std::string ToString() const;
	};

	typedef Vector3<float> Vector3f;
	typedef Vector3<float> Rotator;

	template <class T> Vector3<T> operator+(const Vector3<T>& aVector0, const Vector3<T>& aVector1);
	template <class T> Vector3<T> operator*(const Vector3<T>& aVector0, const Vector3<T>& aVector1);
	template <class T> Vector3<T> operator-(const Vector3<T>& aVector0, const Vector3<T>& aVector1);
	template <class T> Vector3<T> operator*(const Vector3<T>& aVector, const T& aScalar);
	template <class T> Vector3<T> operator*(const T& aScalar, const Vector3<T>& aVector);

	template <class T> Vector3<T> operator/(const Vector3<T>& aVector, const T& aScalar);
	template <class T> void operator+=(Vector3<T>& aVector0, const Vector3<T>& aVector1);
	template <class T> void operator-=(Vector3<T>& aVector0, const Vector3<T>& aVector1);
	template <class T> void operator*=(Vector3<T>& aVector, const T& aScalar);
	template <class T> void operator/=(Vector3<T>& aVector, const T& aScalar);
	template <class T> bool operator==(const Vector3<T>& aVector0, const Vector3<T>& aVector1);
	template <class T> bool operator!=(const Vector3<T>& aVector0, const Vector3<T>& aVector1);
	template <class T> std::istream& operator>>(std::istream& in, Vector3<T>& aVec);
	template <class T> std::ostream& operator>>(std::ostream& out, const Vector3<T>& aVec);
	template <class T> bool operator<(const Vector3<T>& aVector0, const Vector3<T>& aVector1);
#pragma region MemberDefinitions

	template<class T>
	bool operator<(const Vector3<T>& aVector0, const Vector3<T>& aVector1)
	{
		return std::tie(aVector0.X, aVector0.Y, aVector0.Z) < std::tie(aVector1.X, aVector1.Y, aVector1.Z);
	}

	template<class T>
	std::istream& operator>>(std::istream& in, Vector3<T>& aVec)
	{
		in >> aVec.X;
		in >> aVec.Y;
		in >> aVec.Z;
		return in;
	}
	template<class T>
	std::ostream& operator<<(std::ostream& out, const Vector3<T>& aVec)
	{
		out << aVec.X << " ";
		out << aVec.Y << " ";
		out << aVec.Z << " ";
		return out;
	}

	template<class T>
	inline Vector3<T>::Vector3( ) : Vector3(0, 0, 0)
	{
	}

	template<class T>
	inline Vector3<T>::Vector3(const T& aX, const T& aY, const T& aZ) : X(aX), Y(aY), Z(aZ)
	{
	}

	template<class T>
	inline Vector3<T>::Vector3(const T& aScalar) : X(aScalar), Y(aScalar), Z(aScalar)
	{
	}

	template<class T>
	inline Vector3<T>::Vector3(const Vector4<T>& aVector4)
	{
		X = aVector4.X;
		Y = aVector4.Y;
		Z = aVector4.Z;
	}
	template<class T>
	inline Vector3<T>::Vector3(const Vector2<T>& aXY, const T& aZ)
	{
		X = aXY.X;
		Y = aXY.Y;
		//xy = aXY;
		Z = aZ;
	}
	template<class T>
	inline T Vector3<T>::LengthSqr( ) const
	{
		return abs((X * X) + (Y * Y) + (Z * Z));
	}

	template<class T>
	inline T Vector3<T>::Length( ) const
	{
		return abs(sqrt((X * X) + (Y * Y) + (Z * Z)));
	}

	template<class T>
	inline Vector3<T> Vector3<T>::GetNormalized( ) const
	{
		const T magnitude = (X * X) + (Y * Y) + (Z * Z);

		auto sqrtmag = sqrt(magnitude);
		if (sqrtmag == 0)
		{
			return { 0,0,0 };
		}

		const T inversedMagnitude = T(1) / sqrtmag;
		return Vector3<T>(X * inversedMagnitude, Y * inversedMagnitude, Z * inversedMagnitude);
	}

	template<class T>
	inline void Vector3<T>::Normalize( )
	{
		const T magnitude = (X * X) + (Y * Y) + (Z * Z);

#ifdef _DEBUG
		//assert(magnitude != 0 && "Tried to normalize a null vector ");
		if (magnitude == 0) return;
#endif // _DEBUG

		const T inversedMagnitude = T(1) / sqrt(magnitude);
		X = X * inversedMagnitude;
		Y = Y * inversedMagnitude;
		Z = Z * inversedMagnitude;
	}

	template<class T>
	inline T Vector3<T>::Dot(const Vector3<T>& aVector) const
	{
		return (X * aVector.X) + (Y * aVector.Y) + (Z * aVector.Z);
	}

	template<class T>
	inline Vector3<T> Vector3<T>::Cross(const Vector3<T>& aVector) const
	{
		return
		{
			(Y * aVector.Z) - (Z * aVector.Y),
			(Z * aVector.X) - (X * aVector.Z),
			(X * aVector.Y) - (Y * aVector.X)
		};
	}
#pragma endregion MemberDefinitions

#pragma region OperatorDefinitions

	template <class T>
	Vector3<T> operator+(const Vector3<T>& aVector0, const Vector3<T>& aVector1)
	{
		return Vector3<T>(aVector0.X + aVector1.X, aVector0.Y + aVector1.Y, aVector0.Z + aVector1.Z);
	}

	template<class T>
	Vector3<T> operator-(const Vector3<T>& aVector0, const Vector3<T>& aVector1)
	{
		return Vector3<T>(aVector0.X - aVector1.X, aVector0.Y - aVector1.Y, aVector0.Z - aVector1.Z);
	}
	template <class T>
	Vector3<T> operator*(const Vector3<T>& aVector, const Vector3<T>& aVector1)
	{
		return Vector3<T>(aVector.X * aVector1.X, aVector.Y * aVector1.Y, aVector.Z * aVector1.Z);
	}

	template <class T>
	Vector3<T> operator*(const Vector3<T>& aVector, const T& aScalar)
	{
		return Vector3<T>(aVector.X * aScalar, aVector.Y * aScalar, aVector.Z * aScalar);
	}

	template <class T>
	Vector3<T> operator*(const T& aScalar, const Vector3<T>& aVector)
	{
		return aVector * aScalar;
	}

	template <class T>
	Vector3<T> operator/(const Vector3<T>& aVector, const T& aScalar)
	{
		return aVector * (1 / aScalar);
	}

	template <class T>
	void operator+=(Vector3<T>& aVector0, const Vector3<T>& aVector1)
	{
		aVector0.X += aVector1.X;
		aVector0.Y += aVector1.Y;
		aVector0.Z += aVector1.Z;
	}

	template <class T>
	void operator-=(Vector3<T>& aVector0, const Vector3<T>& aVector1)
	{
		aVector0.X -= aVector1.X;
		aVector0.Y -= aVector1.Y;
		aVector0.Z -= aVector1.Z;
	}

	template <class T>
	void operator*=(Vector3<T>& aVector, const T& aScalar)
	{
		aVector.X *= aScalar;
		aVector.Y *= aScalar;
		aVector.Z *= aScalar;
	}

	template <class T>
	void operator/=(Vector3<T>& aVector, const T& aScalar)
	{
		//const T inv = (1 / aScalar);
		aVector.X /= aScalar;
		aVector.Y /= aScalar;
		aVector.Z /= aScalar;
	}

	template <class T>
	bool operator==(const Vector3<T>& aVector0, const Vector3<T>& aVector1)
	{
		return(aVector0.X == aVector1.X && aVector0.Y == aVector1.Y && aVector0.Z == aVector1.Z);
	}

	template <class T>
	bool operator!=(const Vector3<T>& aVector0, const Vector3<T>& aVector1)
	{
		return!(aVector0 == aVector1);
	}

#pragma endregion OperatorDefinitions

#pragma region Static Functions

	template<class T>
	inline Vector3<T> Vector3<T>::Abs(const Vector3<T>& aVector)
	{
		return { std::abs(aVector.X), std::abs(aVector.Y), std::abs(aVector.Z) };
	}

	template <class T>
	inline T Vector3<T>::Distance(const Vector3<T>& aVector0, const Vector3<T>& aVector1)
	{
		const Vector3<T> direction = aVector1 - aVector0;
		return direction.Length( );
	}
	template<class T>
	inline Vector3<T> Vector3<T>::Lerp(const Vector3<T>& aStart, const Vector3<T>& aEnd, const float aPercent)
	{
		return (aStart + aPercent * (aEnd - aStart));
	}
	template<class T>
	inline Vector3<T> Vector3<T>::NLerp(const Vector3<T>& aStart, const Vector3<T>& aEnd, const float aPercent)
	{
		return Lerp(aStart, aEnd, aPercent).Normalize( );
	}

	template <class T>
	std::string Vector3<T>::ToString() const
	{
		return "{ X: " + std::to_string(X) + " Y: " + std::to_string(Y) + " Z: " + std::to_string(Z) + " }";
	}

#pragma endregion Static Functions

	template<typename T>
	const Vector3<T> Vector3<T>::Zero(0, 0, 0);
	template<typename T>
	const Vector3<T> Vector3<T>::One(1, 1, 1);
	template<typename T>
	const Vector3<T> Vector3<T>::Forward(0, 0, 1);
	template<typename T>
	const Vector3<T> Vector3<T>::Right(1, 0, 0);
	template<typename T>
	const Vector3<T> Vector3<T>::Up(0, 1, 0);


#pragma warning(disable : 6385) // Buffer underrun warning.
#pragma warning(disable : 26495) // Uninitialized warning.

	template<typename T>
	class Matrix4x4
	{
	public:
		Matrix4x4<T>( );
		Matrix4x4<T>(const Matrix4x4<T>& aMatrix);
		Matrix4x4<T>(std::initializer_list<T> aList);
		Matrix4x4<T>(float* aList);

		Matrix4x4<T>& operator=(const Matrix4x4<T>& aMatrix);

		T& operator()(const int aRow, const int aColumn);
		const T& operator()(const int aRow, const int aColumn) const;
		bool operator==(const Matrix4x4<T>& aMatrix) const;
		bool operator!=(const Matrix4x4<T>& aMatrix) const;

		Vector4<T> operator*(const Vector4<T>& aVector) const;
		Matrix4x4<T> operator+(const Matrix4x4<T>& aMatrix) const;
		Matrix4x4<T> operator-(const Matrix4x4<T>& aMatrix) const;
		Matrix4x4<T> operator*(const Matrix4x4<float>& aRightMatrix) const;
		Matrix4x4<T> operator*(const T& aScalar) const;
		Matrix4x4<T>& operator+=(const Matrix4x4<T>& aMatrix);
		Matrix4x4<T>& operator-=(const Matrix4x4<T>& aMatrix);
		Matrix4x4<T>& operator*=(const Matrix4x4<T>& aMatrix);
		Matrix4x4<T>& operator*=(const T& aScalar);

		Vector3<T> GetForward( ) const;
		Vector3<T> GetUp( ) const;
		Vector3<T> GetRight( ) const;
		Vector3<T> GetPosition( ) const;
		Vector4<T> GetPositionVec4( ) const;

		void SetIdentity();

		/*This function will decompose the matrix, if this is done every frame and calculated back into a matrix it will twist
		slightly each frame, likely due to precision errors during conversion */
		void DecomposeMatrix(Vector3f& aPosition, Vector3f& aRotation, Vector3f& aScale) const;

		Matrix4x4<T> GetTranspose( ) const;
		Matrix4x4<T> GetInverse( ) const;
		void SetPosition(const Vector3<T>& aVector3);
		void SetForward(const Vector3<T>& aVector3);
		void SetRotation(const Vector3<T>& aVector3);
		void NormalizeXYZ( );
		void Translate(const Vector3<T>& aDirection);

		static Matrix4x4<T> CreateIdentityMatrix( );
		static Matrix4x4<T> CreateTranslationMatrix(Vector3<T> aTranslationVector);
		static Matrix4x4<T> CreateRotationAroundX(T aAngleInRadians);
		static Matrix4x4<T> CreateRotationAroundY(T aAngleInRadians);
		static Matrix4x4<T> CreateRotationAroundZ(T aAngleInRadians);
		static Matrix4x4<T> CreateRollPitchYawMatrix(Vector3<T> aPitchYawRollvector);
		static Matrix4x4<T> CreateScaleMatrix(Vector3<T> aScalarVector);

		static Matrix4x4<T> Transpose(const Matrix4x4<T>& aMatrixToTranspose);
		static Matrix4x4<T> GetFastInverse(const Matrix4x4<T>& aTransform);
		static Matrix4x4<T> Inverse(const Matrix4x4<T>& aMatrixToInverse);

		static Matrix4x4<T> CreateForwardMatrix(Vector3<T> aScalarVector);

		static Matrix4x4<T> CreatePerspectiveMatrixFovX(const T aFov, const T aAspectRatio, const T aNearClip, const T aFarClip);

		static Matrix4x4<T> CreateRotationMatrixFromQuaternionVectorWXYZ(Vector4<T> aVector);
		static Matrix4x4<T> CreateRotationMatrixFromQuaternionVectorXYZW(Vector4<T> aVector);
		static Matrix4x4<T> CreateRotationMatrixFromNormalizedQuaternion(const Quaternion<T>& aQuaternion);

		//Creates a VIEW matrix, NOT a transfomration matrix!!!!!
		static Matrix4x4<T> CreateLookAtDirectionViewMatrix(const Vector3<T>& aEyePosition, const Vector3<T>& aDirection, const Vector3<T>& aUp = Vector3<T>(0.0f, 1.0f, 0.0f));
		static Matrix4x4<T> CreateOrthographicMatrix(const T aLeft, const T aRight, const T aBottom, const T aTop, const T aNear, const T aFar);

		//static Matrix4x4<float> InverseFloat(const Matrix4x4<float>& aMatrixToInverse);
		//static Matrix4x4<float> InverseFastFloat(const Matrix4x4<float>& aMatrixToInverse);

		T* GetDataPtr( ) { return myData; };
	private:
		void ResetRotation( );
		inline T& operator[](const unsigned int& aIndex);
		inline const T& operator[](const unsigned int& aIndex) const;

		static const size_t myLength = 16;
#pragma warning( disable :4201) // Nonstandard nameless struct/union.
		union
		{
			struct
			{
				Vector3<T> myXAxis;
				T myXAxisW;
				Vector3<T> myYAxis;
				T myYAxisW;
				Vector3<T> myZAxis;
				T myZAxisW;
				Vector3<T> myPosition;
				T myPositionW;
			};

			T myData[myLength];
			Vector4<T> myRows[4];
			struct // Special Memory storage for using a fast float inversion, good for animations
			{
				__m128 m1;
				__m128 m2;
				__m128 m3;
				__m128 m4;
			};
			struct // just normal T but follows the standard above as they belong together
			{
				T m11;
				T m21;
				T m31;
				T m41;

				T m12;
				T m22;
				T m32;
				T m42;

				T m13;
				T m23;
				T m33;
				T m43;

				T m14;
				T m24;
				T m34;
				T m44;
			};
		};
#pragma warning( default :4201) // Nonstandard nameless struct/union.
	};

	typedef Matrix4x4<float> Matrix4x4f;

	template<class T>
	Vector4<T> operator*(const Vector4<T>& aVector, const Matrix4x4<T>& aMatrix)
	{
		Vector4<T> result;
		result.X = (aMatrix(1, 1) * aVector.X) + (aMatrix(2, 1) * aVector.Y) + (aMatrix(3, 1) * aVector.Z) + (aMatrix(4, 1) * aVector.W);
		result.Y = (aMatrix(1, 2) * aVector.X) + (aMatrix(2, 2) * aVector.Y) + (aMatrix(3, 2) * aVector.Z) + (aMatrix(4, 2) * aVector.W);
		result.Z = (aMatrix(1, 3) * aVector.X) + (aMatrix(2, 3) * aVector.Y) + (aMatrix(3, 3) * aVector.Z) + (aMatrix(4, 3) * aVector.W);
		result.W = (aMatrix(1, 4) * aVector.X) + (aMatrix(2, 4) * aVector.Y) + (aMatrix(3, 4) * aVector.Z) + (aMatrix(4, 4) * aVector.W);
		return result;
	}

#pragma region Constructors
	template <typename T> Matrix4x4<T>::Matrix4x4( )
	{
		std::memset(myData, 0, myLength * sizeof(T));
		myData[0] = 1;
		myData[5] = 1;
		myData[10] = 1;
		myData[15] = 1;
	}
	template <typename T> Matrix4x4<T>::Matrix4x4(const Matrix4x4<T>& aMatrix)
	{
		std::memcpy(myData, aMatrix.myData, sizeof(T) * myLength);
	}

	template<typename T> inline Matrix4x4<T>::Matrix4x4(std::initializer_list<T> aList)
	{
		assert(aList.size( ) <= myLength && "Initializer list contains too many values.");
		std::memcpy(myData, aList.begin( ), sizeof(T) * aList.size( ));
		if (aList.size( ) < myLength)
		{
			std::fill(myData + aList.size( ), myData + myLength, myData[aList.size( ) - 1]);
		}
	}

	template<typename T>
	inline Matrix4x4<T>::Matrix4x4(float* aList)
	{
		for (size_t i = 0; i < myLength; i++)
		{
			myData[i] = *(aList + i);
		}
	}
#pragma endregion Constructors

#pragma region Operators

	template<typename T>
	inline Matrix4x4<T>& Matrix4x4<T>::operator=(const Matrix4x4<T>& aMatrix)
	{
		std::memcpy(myData, aMatrix.myData, sizeof(T) * myLength);
		return *this;
	}

	// Rows and Columns start at 1.
	template<typename T> inline T& Matrix4x4<T>::operator()(const int aRow, const int aColumn)
	{
		assert(aRow > 0 && aRow < 5 && aColumn > 0 && aColumn < 5 && "Argument out of bounds");
		return myData[(aRow - 1) * 4 + (aColumn - 1)];
	}

	// Rows and Columns start at 1.
	template<typename T> inline const T& Matrix4x4<T>::operator()(const int aRow, const int aColumn) const
	{
		assert(aRow > 0 && aRow < 5 && aColumn > 0 && aColumn < 5 && "Argument out of bounds");
		return myData[(aRow - 1) * 4 + (aColumn - 1)];
	}

	template<typename T>
	inline bool Matrix4x4<T>::operator==(const Matrix4x4<T>& aMatrix) const
	{
		for (int i = 0; i < myLength; i++)
		{
			if (myData[i] != aMatrix.myData[i])
			{
				return false;
			}
		}
		return true;
	}

	template<typename T>
	inline bool Matrix4x4<T>::operator!=(const Matrix4x4<T>& aMatrix) const
	{
		return !operator==(aMatrix);
	}

	template<typename T>
	inline Matrix4x4<T> Matrix4x4<T>::operator+(const Matrix4x4<T>& aMatrix) const
	{
		Matrix4x4<T> result{ *this };
		return result += aMatrix;
	}

	template<typename T>
	inline Matrix4x4<T>& Matrix4x4<T>::operator+=(const Matrix4x4<T>& aMatrix)
	{
		for (auto i = 0; i < myLength; i++)
		{
			myData[i] += aMatrix.myData[i];
		}
		return *this;
	}

	template<typename T>
	inline Matrix4x4<T> Matrix4x4<T>::operator-(const Matrix4x4<T>& aMatrix) const
	{
		Matrix4x4<T> result{ *this };
		return result -= aMatrix;
	}

	template<typename T>
	inline Matrix4x4<T>& Matrix4x4<T>::operator-=(const Matrix4x4<T>& aMatrix)
	{
		for (auto i = 0; i < myLength; i++)
		{
			myData[i] -= aMatrix.myData[i];
		}
		return *this;
	}

	template<typename T>
	inline Matrix4x4<T> Matrix4x4<T>::operator*(const Matrix4x4<float>& aRightMatrix) const
	{
		Matrix4x4<float> result;
		const __m128& a = aRightMatrix.m1;
		const __m128& b = aRightMatrix.m2;
		const __m128& c = aRightMatrix.m3;
		const __m128& d = aRightMatrix.m4;

		__m128 t1, t2;

		t1 = _mm_set1_ps((*this)[0]);
		t2 = _mm_mul_ps(a, t1);
		t1 = _mm_set1_ps((*this)[1]);
		t2 = _mm_add_ps(_mm_mul_ps(b, t1), t2);
		t1 = _mm_set1_ps((*this)[2]);
		t2 = _mm_add_ps(_mm_mul_ps(c, t1), t2);
		t1 = _mm_set1_ps((*this)[3]);
		t2 = _mm_add_ps(_mm_mul_ps(d, t1), t2);

		_mm_store_ps(&result[0], t2);

		t1 = _mm_set1_ps((*this)[4]);
		t2 = _mm_mul_ps(a, t1);
		t1 = _mm_set1_ps((*this)[5]);
		t2 = _mm_add_ps(_mm_mul_ps(b, t1), t2);
		t1 = _mm_set1_ps((*this)[6]);
		t2 = _mm_add_ps(_mm_mul_ps(c, t1), t2);
		t1 = _mm_set1_ps((*this)[7]);
		t2 = _mm_add_ps(_mm_mul_ps(d, t1), t2);

		_mm_store_ps(&result[4], t2);

		t1 = _mm_set1_ps((*this)[8]);
		t2 = _mm_mul_ps(a, t1);
		t1 = _mm_set1_ps((*this)[9]);
		t2 = _mm_add_ps(_mm_mul_ps(b, t1), t2);
		t1 = _mm_set1_ps((*this)[10]);
		t2 = _mm_add_ps(_mm_mul_ps(c, t1), t2);
		t1 = _mm_set1_ps((*this)[11]);
		t2 = _mm_add_ps(_mm_mul_ps(d, t1), t2);

		_mm_store_ps(&result[8], t2);

		t1 = _mm_set1_ps((*this)[12]);
		t2 = _mm_mul_ps(a, t1);
		t1 = _mm_set1_ps((*this)[13]);
		t2 = _mm_add_ps(_mm_mul_ps(b, t1), t2);
		t1 = _mm_set1_ps((*this)[14]);
		t2 = _mm_add_ps(_mm_mul_ps(c, t1), t2);
		t1 = _mm_set1_ps((*this)[15]);
		t2 = _mm_add_ps(_mm_mul_ps(d, t1), t2);

		_mm_store_ps(&result[12], t2);
		return result;
	}

	template<typename T>
	inline Matrix4x4<T>& Matrix4x4<T>::operator*=(const Matrix4x4<T>& aMatrix)
	{
		Matrix4x4<T> result;
		for (int i = 1; i <= 4; i++)
		{
			for (auto j = 1; j <= 4; j++)
			{
				T product{ 0 };
				for (auto k = 1; k <= 4; k++)
				{
					product += this->operator()(i, k) * aMatrix(k, j);
				}
				result(i, j) = product;
			}
		}
		std::memcpy(this->myData, result.myData, sizeof(T) * myLength);
		return *this;
	}

	template<typename T>
	inline Matrix4x4<T> Matrix4x4<T>::operator*(const T& aScalar) const
	{
		Matrix4x4<T> result{ *this };
		return result *= aScalar;
	}
	template<typename T>
	inline Matrix4x4<T>& Matrix4x4<T>::operator*=(const T& aScalar)
	{
		for (int i = 0; i < myLength; i++)
		{
			myData[i] *= aScalar;
		}
		return *this;
	}

	template<typename T>
	inline Vector4<T> Matrix4x4<T>::operator*(const Vector4<T>& aVector) const
	{
		Vector4<T> result;
		result.X = (myData[0] * aVector.X) + (myData[4] * aVector.Y) + (myData[8] * aVector.Z) + (myData[12] * aVector.W);
		result.Y = (myData[1] * aVector.X) + (myData[5] * aVector.Y) + (myData[9] * aVector.Z) + (myData[13] * aVector.W);
		result.Z = (myData[2] * aVector.X) + (myData[6] * aVector.Y) + (myData[10] * aVector.Z) + (myData[14] * aVector.W);
		result.W = (myData[3] * aVector.X) + (myData[7] * aVector.Y) + (myData[11] * aVector.Z) + (myData[15] * aVector.W);
		return result;
	}
#pragma endregion Operators
	template<typename T>
	inline Vector3<T> Matrix4x4<T>::GetForward( ) const
	{
		Matrix4x4<T> rotation = *this;
		rotation(4, 1) = 0;
		rotation(4, 2) = 0;
		rotation(4, 3) = 0;
		auto result = rotation * Vector4<T> { 0, 0, 1.f, 1.f };
		return { result.X, result.Y, result.Z };
	}

	template<typename T>
	inline Vector3<T> Matrix4x4<T>::GetUp( ) const
	{
		Matrix4x4<T> rotation = *this;
		rotation(4, 1) = 0;
		rotation(4, 2) = 0;
		rotation(4, 3) = 0;
		auto result = rotation * Vector4<T> { 0, 1.f, 0, 1.f };
		return { result.X, result.Y, result.Z };
	}

	template<typename T>
	inline Vector3<T> Matrix4x4<T>::GetRight( ) const
	{
		Matrix4x4<T> rotation = *this;
		rotation(4, 1) = 0;
		rotation(4, 2) = 0;
		rotation(4, 3) = 0;
		auto result = rotation * Vector4<T> { 1.f, 0, 0, 1.f };
		return { result.X, result.Y, result.Z };
	}
	template<typename T>
	inline Vector3<T> Matrix4x4<T>::GetPosition( ) const
	{
		return { (*this)(4,1), (*this)(4,2), (*this)(4,3) };
	}

	template<typename T>
	inline Vector4<T> Matrix4x4<T>::GetPositionVec4( ) const
	{
		return { (*this)(4,1), (*this)(4,2), (*this)(4,3), (*this)(4,4) };
	}

template <typename T>
void Matrix4x4<T>::SetIdentity()
{
	T idData[] = {1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1};		
	std::memcpy(myData, idData, sizeof(T) * 16);
}

template<typename T>
	inline void Matrix4x4<T>::DecomposeMatrix(Vector3f& aPosition, Vector3f& aRotation, Vector3f& aScale) const
	{
		static const float ZPI = 3.1415f;
		static const float RAD2DEG = (180.f / ZPI);
		const Matrix4x4<T>& mat = *this;
		Vector4f scaleX ={ mat(1,1),mat(1,2) ,mat(1,3) ,mat(1,4) };
		Vector4f scaleY ={ mat(2,1),mat(2,2) ,mat(2,3) ,mat(2,4) };
		Vector4f scaleZ ={ mat(3,1),mat(3,2) ,mat(3,3) ,mat(3,4) };

		aScale.X = scaleX.Length( );
		aScale.Y = scaleY.Length( );
		aScale.Z = scaleZ.Length( );

		scaleX.Normalize( );
		scaleY.Normalize( );
		scaleZ.Normalize( );

		aRotation.X = RAD2DEG * atan2f(scaleY.Z, scaleZ.Z);
		aRotation.Y = RAD2DEG * atan2f(-scaleX.Z, sqrtf(scaleY.Z * scaleY.Z + scaleZ.Z * scaleZ.Z));
		aRotation.Z = RAD2DEG * atan2f(scaleX.Y, scaleX.X);

		aPosition.X = mat.GetPosition( ).X;
		aPosition.Y = mat.GetPosition( ).Y;
		aPosition.Z = mat.GetPosition( ).Z;
	}

	template<typename T>
	inline Matrix4x4<T> Matrix4x4<T>::GetTranspose( ) const
	{
		return Transpose(*this);;
	}

	template<typename T>
	inline Matrix4x4<T> Matrix4x4<T>::GetInverse( ) const
	{
		return Inverse(*this);;
	}
	template<typename T>
	inline void Matrix4x4<T>::NormalizeXYZ( )
	{
		const T magnitude = (myXAxis.X * myXAxis.X) + (myXAxis.Y * myXAxis.Y) + (myXAxis.Z * myXAxis.Z);
		const T magnitude1 = (myYAxis.X * myYAxis.X) + (myYAxis.Y * myYAxis.Y) + (myYAxis.Z * myYAxis.Z);
		const T magnitude2 = (myZAxis.X * myZAxis.X) + (myZAxis.Y * myZAxis.Y) + (myZAxis.Z * myZAxis.Z);
		if (magnitude == 0 && magnitude1 == 0 && magnitude2 == 0)
		{
			return;
		}
		myXAxis.Normalize( );
		myYAxis.Normalize( );
		myZAxis.Normalize( );
	}
	template<typename T>
	void Matrix4x4<T>::Translate(const Vector3<T>& aDirection)
	{
		myData[12] += aDirection.X;
		myData[13] += aDirection.Y;
		myData[14] += aDirection.Z;
	}

	template <class T>
	inline T& Matrix4x4<T>::operator[](const unsigned int& aIndex)
	{
		assert((aIndex < 16) && "Index out of bounds.");
		return myData[aIndex];
	}

	template <class T>
	const inline T& Matrix4x4<T>::operator[](const unsigned int& aIndex) const
	{
		assert((aIndex < 16) && "Index out of bounds.");
		return myData[aIndex];
	}

#pragma region Static Functions

	template<typename T>
	inline Matrix4x4<T> Matrix4x4<T>::CreateIdentityMatrix( )
	{
		return
		{
			T(1),0,0,0,
			0,T(1),0,0,
			0,0,T(1),0,
			0,0,0,T(1)
		};
	}

	template<typename T>
	inline Matrix4x4<T> Matrix4x4<T>::CreateRotationAroundX(const T aAngleInRadians)
	{
		const T cos = std::cos(aAngleInRadians);
		const T sin = std::sin(aAngleInRadians);
		return
		{
			T(1), 0,  0,  0,
			0,  cos, sin, 0,
			0, -sin, cos, 0,
			0,  0,  0,  T(1)
		};
	}

	template<typename T>
	inline Matrix4x4<T> Matrix4x4<T>::CreateRotationAroundY(const T aAngleInRadians)
	{
		const T cos = std::cos(aAngleInRadians);
		const T sin = std::sin(aAngleInRadians);
		return
		{
			cos, 0, -sin, 0,
			0, T(1), 0,   0,
			sin, 0,  cos, 0,
			0,   0,  0, T(1)
		};
	}

	template<typename T>
	inline Matrix4x4<T> Matrix4x4<T>::CreateRotationAroundZ(const T aAngleInRadians)
	{
		const T cos = std::cos(aAngleInRadians);
		const T sin = std::sin(aAngleInRadians);
		return
		{
			cos,  sin, 0, 0,
			-sin, cos, 0, 0,
			0,    0, T(1),0,
			0,    0, 0, T(1)
		};
	}

	template<typename T>
	inline Matrix4x4<T> Matrix4x4<T>::CreateForwardMatrix(Vector3<T> aScalarVector)
	{
		const float radConst = 3.141f / 180.f;
		aScalarVector *= radConst;

		Matrix4x4<T> rotation;
		rotation.myZAxis = aScalarVector;
		return rotation;
	}

	template<typename T>
	inline Matrix4x4<T> Matrix4x4<T>::Transpose(const Matrix4x4<T>& aMatrixToTranspose)
	{
		Matrix4x4<T> result;
		for (int i = 0; i < 4; i++)
		{
			for (int j = 0; j < 4; j++)
			{
				result.myData[i + j * 4] = aMatrixToTranspose.myData[j + i * 4];
			}
		}
		return result;
	}

	template<typename T>
	inline Matrix4x4<T> Matrix4x4<T>::GetFastInverse(const Matrix4x4<T>& aTransform)
	{
		Matrix4x4<T> inv;

		inv[0] = aTransform[0];
		inv[1] = aTransform[4];
		inv[2] = aTransform[8];
		//inv.a3 = aTransform[12];
		inv[4] = aTransform[1];
		inv[5] = aTransform[5];
		inv[6] = aTransform[9];
		//inv.a7 = aTransform[13];
		inv[8] = aTransform[2];
		inv[9] = aTransform[6];
		inv[10] = aTransform[10];
		//inv[1]1 = aTransform[14];
		inv[15] = aTransform[15];

		inv[12] = ((-aTransform[12]) * inv[0]) + ((-aTransform[13]) * inv[4]) + ((-aTransform[14]) * inv[8]);
		inv[13] = ((-aTransform[12]) * inv[1]) + ((-aTransform[13]) * inv[5]) + ((-aTransform[14]) * inv[9]);
		inv[14] = ((-aTransform[12]) * inv[2]) + ((-aTransform[13]) * inv[6]) + ((-aTransform[14]) * inv[10]);

		return inv;
	}

	template<class T>
	inline Matrix4x4<T> Matrix4x4<T>::Inverse(const Matrix4x4<T>& aMatrixToInverse)
	{
		T inv[16];

		inv[0] =
			aMatrixToInverse[5] * aMatrixToInverse[10] * aMatrixToInverse[15] -
			aMatrixToInverse[5] * aMatrixToInverse[11] * aMatrixToInverse[14] -
			aMatrixToInverse[9] * aMatrixToInverse[6] * aMatrixToInverse[15] +
			aMatrixToInverse[9] * aMatrixToInverse[7] * aMatrixToInverse[14] +
			aMatrixToInverse[13] * aMatrixToInverse[6] * aMatrixToInverse[11] -
			aMatrixToInverse[13] * aMatrixToInverse[7] * aMatrixToInverse[10];

		inv[4] = -aMatrixToInverse[4] * aMatrixToInverse[10] * aMatrixToInverse[15] +
			aMatrixToInverse[4] * aMatrixToInverse[11] * aMatrixToInverse[14] +
			aMatrixToInverse[8] * aMatrixToInverse[6] * aMatrixToInverse[15] -
			aMatrixToInverse[8] * aMatrixToInverse[7] * aMatrixToInverse[14] -
			aMatrixToInverse[12] * aMatrixToInverse[6] * aMatrixToInverse[11] +
			aMatrixToInverse[12] * aMatrixToInverse[7] * aMatrixToInverse[10];

		inv[8] = aMatrixToInverse[4] * aMatrixToInverse[9] * aMatrixToInverse[15] -
			aMatrixToInverse[4] * aMatrixToInverse[11] * aMatrixToInverse[13] -
			aMatrixToInverse[8] * aMatrixToInverse[5] * aMatrixToInverse[15] +
			aMatrixToInverse[8] * aMatrixToInverse[7] * aMatrixToInverse[13] +
			aMatrixToInverse[12] * aMatrixToInverse[5] * aMatrixToInverse[11] -
			aMatrixToInverse[12] * aMatrixToInverse[7] * aMatrixToInverse[9];

		inv[12] = -aMatrixToInverse[4] * aMatrixToInverse[9] * aMatrixToInverse[14] +
			aMatrixToInverse[4] * aMatrixToInverse[10] * aMatrixToInverse[13] +
			aMatrixToInverse[8] * aMatrixToInverse[5] * aMatrixToInverse[14] -
			aMatrixToInverse[8] * aMatrixToInverse[6] * aMatrixToInverse[13] -
			aMatrixToInverse[12] * aMatrixToInverse[5] * aMatrixToInverse[10] +
			aMatrixToInverse[12] * aMatrixToInverse[6] * aMatrixToInverse[9];

		inv[1] = -aMatrixToInverse[1] * aMatrixToInverse[10] * aMatrixToInverse[15] +
			aMatrixToInverse[1] * aMatrixToInverse[11] * aMatrixToInverse[14] +
			aMatrixToInverse[9] * aMatrixToInverse[2] * aMatrixToInverse[15] -
			aMatrixToInverse[9] * aMatrixToInverse[3] * aMatrixToInverse[14] -
			aMatrixToInverse[13] * aMatrixToInverse[2] * aMatrixToInverse[11] +
			aMatrixToInverse[13] * aMatrixToInverse[3] * aMatrixToInverse[10];

		inv[5] = aMatrixToInverse[0] * aMatrixToInverse[10] * aMatrixToInverse[15] -
			aMatrixToInverse[0] * aMatrixToInverse[11] * aMatrixToInverse[14] -
			aMatrixToInverse[8] * aMatrixToInverse[2] * aMatrixToInverse[15] +
			aMatrixToInverse[8] * aMatrixToInverse[3] * aMatrixToInverse[14] +
			aMatrixToInverse[12] * aMatrixToInverse[2] * aMatrixToInverse[11] -
			aMatrixToInverse[12] * aMatrixToInverse[3] * aMatrixToInverse[10];

		inv[9] = -aMatrixToInverse[0] * aMatrixToInverse[9] * aMatrixToInverse[15] +
			aMatrixToInverse[0] * aMatrixToInverse[11] * aMatrixToInverse[13] +
			aMatrixToInverse[8] * aMatrixToInverse[1] * aMatrixToInverse[15] -
			aMatrixToInverse[8] * aMatrixToInverse[3] * aMatrixToInverse[13] -
			aMatrixToInverse[12] * aMatrixToInverse[1] * aMatrixToInverse[11] +
			aMatrixToInverse[12] * aMatrixToInverse[3] * aMatrixToInverse[9];

		inv[13] = aMatrixToInverse[0] * aMatrixToInverse[9] * aMatrixToInverse[14] -
			aMatrixToInverse[0] * aMatrixToInverse[10] * aMatrixToInverse[13] -
			aMatrixToInverse[8] * aMatrixToInverse[1] * aMatrixToInverse[14] +
			aMatrixToInverse[8] * aMatrixToInverse[2] * aMatrixToInverse[13] +
			aMatrixToInverse[12] * aMatrixToInverse[1] * aMatrixToInverse[10] -
			aMatrixToInverse[12] * aMatrixToInverse[2] * aMatrixToInverse[9];

		inv[2] = aMatrixToInverse[1] * aMatrixToInverse[6] * aMatrixToInverse[15] -
			aMatrixToInverse[1] * aMatrixToInverse[7] * aMatrixToInverse[14] -
			aMatrixToInverse[5] * aMatrixToInverse[2] * aMatrixToInverse[15] +
			aMatrixToInverse[5] * aMatrixToInverse[3] * aMatrixToInverse[14] +
			aMatrixToInverse[13] * aMatrixToInverse[2] * aMatrixToInverse[7] -
			aMatrixToInverse[13] * aMatrixToInverse[3] * aMatrixToInverse[6];

		inv[6] = -aMatrixToInverse[0] * aMatrixToInverse[6] * aMatrixToInverse[15] +
			aMatrixToInverse[0] * aMatrixToInverse[7] * aMatrixToInverse[14] +
			aMatrixToInverse[4] * aMatrixToInverse[2] * aMatrixToInverse[15] -
			aMatrixToInverse[4] * aMatrixToInverse[3] * aMatrixToInverse[14] -
			aMatrixToInverse[12] * aMatrixToInverse[2] * aMatrixToInverse[7] +
			aMatrixToInverse[12] * aMatrixToInverse[3] * aMatrixToInverse[6];

		inv[10] = aMatrixToInverse[0] * aMatrixToInverse[5] * aMatrixToInverse[15] -
			aMatrixToInverse[0] * aMatrixToInverse[7] * aMatrixToInverse[13] -
			aMatrixToInverse[4] * aMatrixToInverse[1] * aMatrixToInverse[15] +
			aMatrixToInverse[4] * aMatrixToInverse[3] * aMatrixToInverse[13] +
			aMatrixToInverse[12] * aMatrixToInverse[1] * aMatrixToInverse[7] -
			aMatrixToInverse[12] * aMatrixToInverse[3] * aMatrixToInverse[5];

		inv[14] = -aMatrixToInverse[0] * aMatrixToInverse[5] * aMatrixToInverse[14] +
			aMatrixToInverse[0] * aMatrixToInverse[6] * aMatrixToInverse[13] +
			aMatrixToInverse[4] * aMatrixToInverse[1] * aMatrixToInverse[14] -
			aMatrixToInverse[4] * aMatrixToInverse[2] * aMatrixToInverse[13] -
			aMatrixToInverse[12] * aMatrixToInverse[1] * aMatrixToInverse[6] +
			aMatrixToInverse[12] * aMatrixToInverse[2] * aMatrixToInverse[5];

		inv[3] = -aMatrixToInverse[1] * aMatrixToInverse[6] * aMatrixToInverse[11] +
			aMatrixToInverse[1] * aMatrixToInverse[7] * aMatrixToInverse[10] +
			aMatrixToInverse[5] * aMatrixToInverse[2] * aMatrixToInverse[11] -
			aMatrixToInverse[5] * aMatrixToInverse[3] * aMatrixToInverse[10] -
			aMatrixToInverse[9] * aMatrixToInverse[2] * aMatrixToInverse[7] +
			aMatrixToInverse[9] * aMatrixToInverse[3] * aMatrixToInverse[6];

		inv[7] = aMatrixToInverse[0] * aMatrixToInverse[6] * aMatrixToInverse[11] -
			aMatrixToInverse[0] * aMatrixToInverse[7] * aMatrixToInverse[10] -
			aMatrixToInverse[4] * aMatrixToInverse[2] * aMatrixToInverse[11] +
			aMatrixToInverse[4] * aMatrixToInverse[3] * aMatrixToInverse[10] +
			aMatrixToInverse[8] * aMatrixToInverse[2] * aMatrixToInverse[7] -
			aMatrixToInverse[8] * aMatrixToInverse[3] * aMatrixToInverse[6];

		inv[11] = -aMatrixToInverse[0] * aMatrixToInverse[5] * aMatrixToInverse[11] +
			aMatrixToInverse[0] * aMatrixToInverse[7] * aMatrixToInverse[9] +
			aMatrixToInverse[4] * aMatrixToInverse[1] * aMatrixToInverse[11] -
			aMatrixToInverse[4] * aMatrixToInverse[3] * aMatrixToInverse[9] -
			aMatrixToInverse[8] * aMatrixToInverse[1] * aMatrixToInverse[7] +
			aMatrixToInverse[8] * aMatrixToInverse[3] * aMatrixToInverse[5];

		inv[15] = aMatrixToInverse[0] * aMatrixToInverse[5] * aMatrixToInverse[10] -
			aMatrixToInverse[0] * aMatrixToInverse[6] * aMatrixToInverse[9] -
			aMatrixToInverse[4] * aMatrixToInverse[1] * aMatrixToInverse[10] +
			aMatrixToInverse[4] * aMatrixToInverse[2] * aMatrixToInverse[9] +
			aMatrixToInverse[8] * aMatrixToInverse[1] * aMatrixToInverse[6] -
			aMatrixToInverse[8] * aMatrixToInverse[2] * aMatrixToInverse[5];

		T det = aMatrixToInverse[0] * inv[0] + aMatrixToInverse[1] * inv[4] + aMatrixToInverse[2] * inv[8] + aMatrixToInverse[3] * inv[12];

		//if (det == 0)
		//	return false;

		det = T(1.0) / det;

		Matrix4x4<T> returnMatrix;
		for (int i = 0; i < 16; i++)
		{
			returnMatrix[i] = inv[i] * det;
		}

		return returnMatrix;
	}

	template<typename T>
	inline Matrix4x4<T> Matrix4x4<T>::CreateScaleMatrix(Vector3<T> aScalarVector)
	{
		Matrix4x4<T> result;
		result.myData[0] = aScalarVector.X;
		result.myData[5] = aScalarVector.Y;
		result.myData[10] = aScalarVector.Z;
		return result;
	}

	template<typename T>
	inline Matrix4x4<T> Matrix4x4<T>::CreateRollPitchYawMatrix(Vector3<T> aPitchYawRollvector)
	{
		//pitch * roll * yaw

		const float radConst = 3.141f / 180.f;
		aPitchYawRollvector *= radConst;

		Quaternion<T> rotation = Quaternion<T>(aPitchYawRollvector);
		Vector4<T> values ={ rotation.W, rotation.X, rotation.Y, rotation.Z };
		return CreateRotationMatrixFromQuaternionVectorWXYZ(values);
	}

	template<typename T>
	inline Matrix4x4<T> Matrix4x4<T>::CreateTranslationMatrix(Vector3<T> aTranslationVector)
	{
		Matrix4x4<T> result;

		result.myPosition = aTranslationVector;
		return result;
	}

	/// <summary>
	/// Order: X, Y, Z, W
	/// </summary>
	template<class T>
	inline Matrix4x4<T> Matrix4x4<T>::CreateRotationMatrixFromQuaternionVectorXYZW(Vector4<T> aVector)
	{
		Matrix4x4<T> result;
		Vector4<T> v ={ aVector.W, aVector.X, aVector.Y, aVector.Z };
		aVector = v;
		T qxx(aVector.Y * aVector.Y);
		T qyy(aVector.Z * aVector.Z);
		T qzz(aVector.W * aVector.W);

		T qxz(aVector.Y * aVector.W);
		T qxy(aVector.Y * aVector.Z);
		T qyz(aVector.Z * aVector.W);

		T qwx(aVector.X * aVector.Y);
		T qwy(aVector.X * aVector.Z);
		T qwz(aVector.X * aVector.W);

		result.myXAxis.X = T(1) - T(2) * (qyy + qzz);
		result.myXAxis.Y = T(2) * (qxy + qwz);
		result.myXAxis.Z = T(2) * (qxz - qwy);

		result.myYAxis.X = T(2) * (qxy - qwz);
		result.myYAxis.Y = T(1) - T(2) * (qxx + qzz);
		result.myYAxis.Z = T(2) * (qyz + qwx);

		result.myZAxis.X = T(2) * (qxz + qwy);
		result.myZAxis.Y = T(2) * (qyz - qwx);
		result.myZAxis.Z = T(1) - T(2) * (qxx + qyy);
		return result;
	}
	template<typename T>
	inline Matrix4x4<T> Matrix4x4<T>::CreateRotationMatrixFromNormalizedQuaternion(const Quaternion<T>& aQuaternion)
	{
		T xx = aQuaternion.X * aQuaternion.X;
		T xy = aQuaternion.X * aQuaternion.Y;
		T xz = aQuaternion.X * aQuaternion.Z;
		T xw = aQuaternion.X * aQuaternion.W;

		T yy = aQuaternion.Y * aQuaternion.Y;
		T yz = aQuaternion.Y * aQuaternion.Z;
		T yw = aQuaternion.Y * aQuaternion.W;

		T zz = aQuaternion.Z * aQuaternion.Z;
		T zw = aQuaternion.Z * aQuaternion.W;

		Matrix4x4<T> result;

		result.m11 = T(1) - T(2) * (yy + zz);
		result.m12 = T(2) * (xy - zw);
		result.m13 = T(2) * (xz + yw);

		result.m21 = 2 * (xy + zw);
		result.m22 = 1 - 2 * (xx + zz);
		result.m23 = 2 * (yz - xw);

		result.m31 = 2 * (xz - yw);
		result.m32 = 2 * (yz + xw);
		result.m33 = 1 - 2 * (xx + yy);

		result.m14 = result.m24 = result.m34 = result.m41 = result.m42 = result.m43 = 0;
		result.m44 = 1;
		return result;
	}
	/// <summary>
	/// Order: W, X, Y, Z
	/// </summary>
	template<class T>
	inline Matrix4x4<T> Matrix4x4<T>::CreateRotationMatrixFromQuaternionVectorWXYZ(Vector4<T> aVector)
	{
		Matrix4x4<T> result;

		T qxx(aVector.Y * aVector.Y);
		T qyy(aVector.Z * aVector.Z);
		T qzz(aVector.W * aVector.W);

		T qxz(aVector.Y * aVector.W);
		T qxy(aVector.Y * aVector.Z);
		T qyz(aVector.Z * aVector.W);

		T qwx(aVector.X * aVector.Y);
		T qwy(aVector.X * aVector.Z);
		T qwz(aVector.X * aVector.W);

		result.myXAxis.X = T(1) - T(2) * (qyy + qzz);
		result.myXAxis.Y = T(2) * (qxy + qwz);
		result.myXAxis.Z = T(2) * (qxz - qwy);

		result.myYAxis.X = T(2) * (qxy - qwz);
		result.myYAxis.Y = T(1) - T(2) * (qxx + qzz);
		result.myYAxis.Z = T(2) * (qyz + qwx);

		result.myZAxis.X = T(2) * (qxz + qwy);
		result.myZAxis.Y = T(2) * (qyz - qwx);
		result.myZAxis.Z = T(1) - T(2) * (qxx + qyy);

		result.m41 = result.m42 = result.m43 = 0;
		result.m44 = 1;
		return result;
	}

	template<class T>
	inline Matrix4x4<T> Matrix4x4<T>::CreatePerspectiveMatrixFovX(const T aFov, const T aAspectRatio, const T aNearClip, const T aFarClip)
	{
		float xScale = static_cast<T>(1) / tan(aFov / static_cast<T>(2));
		float yScale = xScale * aAspectRatio;

		Matrix4x4<T> persp;
		persp[0] = xScale;
		persp[5] = yScale;
		persp[10] = aFarClip / (aFarClip - aNearClip);
		persp[11] = T(1);
		persp[14] = (-aNearClip * aFarClip) / (aFarClip - aNearClip);
		persp[15] = 0;
		return persp;
	}

	template<class T>
	inline Matrix4x4<T> Matrix4x4<T>::CreateLookAtDirectionViewMatrix(const Vector3<T>& aEyePosition, const Vector3<T>& aDirection, const Vector3<T>& aUp)
	{
		Vector3<T> xaxis = aUp.Cross(aDirection).GetNormalized( );
		Vector3<T> yaxis = aDirection.Cross(xaxis);
		return {
			xaxis.X, yaxis.X, aDirection.X, T(0),
			xaxis.Y, yaxis.Y, aDirection.Y, T(0),
			xaxis.Z, yaxis.Z, aDirection.Z, T(0),
			-xaxis.Dot(aEyePosition), -yaxis.Dot(aEyePosition), -aDirection.Dot(aEyePosition), T(1)
		};
	}

	template<class T>
	inline Matrix4x4<T> Matrix4x4<T>::CreateOrthographicMatrix(const T aLeft, const T aRight, const T aBottom, const T aTop, const T aNear, const T aFar)
	{
		Matrix4x4<T> result;
		result[0] = T(2) / (aRight - aLeft);
		result[1] = T(0);
		result[2] = T(0);
		result[3] = T(0);

		//Second row
		result[4] = T(0);
		result[5] = T(2) / (aTop - aBottom);
		result[6] = T(0);
		result[7] = T(0);

		//Third row
		result[8] = T(0);
		result[9] = T(0);
		result[10] = T(1) / (aFar - aNear);
		result[11] = T(0);

		//Fourth row
		result[12] = (aLeft + aRight) / (aLeft - aRight);
		result[13] = (aTop + aBottom) / (aBottom - aTop);
		result[14] = aNear / (aNear - aFar);
		result[15] = T(1);
		return result;
	}

	template<class T>
	inline void Matrix4x4<T>::SetPosition(const Vector3<T>& aVector3)
	{
		myData[12] = aVector3.X;
		myData[13] = aVector3.Y;
		myData[14] = aVector3.Z;
	}
	template<typename T>
	inline void Matrix4x4<T>::SetForward(const Vector3<T>& aVector3)
	{
		myData[8] = aVector3.X;
		myData[9] = aVector3.Y;
		myData[10] = aVector3.Z;
	}

	template<class T>
	inline void Matrix4x4<T>::SetRotation(const Vector3<T>& aVector3)
	{
		ResetRotation( );
		*this *= CreateRollPitchYawMatrix(aVector3);
	}

	template<class T>
	inline void Matrix4x4<T>::ResetRotation( )
	{
		myData[5] = 1;
		myData[6] = 0;
		myData[9] = 0;
		myData[10] = 1;
		myData[0] = 1;
		myData[2] = 0;
		myData[8] = 0;
		myData[1] = 0;
		myData[4] = 0;
	}

#pragma endregion Static Functions

	// *****************************************
// *** Helper functions for SIMD inverse ***
// *****************************************
// Source: https://lxjk.github.io/2017/09/03/Fast-4x4-Matrix-Inverse-with-SSE-SIMD-Explained.html
// *****************************************

#define MakeShuffleMask(x,y,z,w)           (x | (y<<2) | (z<<4) | (w<<6))

	// vec(0, 1, 2, 3) -> (vec[X], vec[Y], vec[Z], vec[W])
#define VecSwizzle(vec, x,y,z,w)           _mm_shuffle_ps(vec, vec, MakeShuffleMask(x,y,z,w))
#define VecSwizzle1(vec, x)                _mm_shuffle_ps(vec, vec, MakeShuffleMask(x,x,x,x))
	// special swizzle
#define VecSwizzle_0101(vec)               _mm_movelh_ps(vec, vec)
#define VecSwizzle_2323(vec)               _mm_movehl_ps(vec, vec)
#define VecSwizzle_0022(vec)               _mm_moveldup_ps(vec)
#define VecSwizzle_1133(vec)               _mm_movehdup_ps(vec)

	// return (vec1[X], vec1[Y], vec2[Z], vec2[W])
#define VecShuffle(vec1, vec2, x,y,z,w)    _mm_shuffle_ps(vec1, vec2, MakeShuffleMask(x,y,z,w))
	// special shuffle
#define VecShuffle_0101(vec1, vec2)        _mm_movelh_ps(vec1, vec2)
#define VecShuffle_2323(vec1, vec2)        _mm_movehl_ps(vec2, vec1)

	__forceinline __m128 Mat2Mul(__m128 vec1, __m128 vec2)
	{
		return
			_mm_add_ps(_mm_mul_ps(vec1, VecSwizzle(vec2, 0, 3, 0, 3)),
			_mm_mul_ps(VecSwizzle(vec1, 1, 0, 3, 2), VecSwizzle(vec2, 2, 1, 2, 1)));
	}
	// 2x2 row major Matrix adjugate multiply (A#)*B
	__forceinline __m128 Mat2AdjMul(__m128 vec1, __m128 vec2)
	{
		return
			_mm_sub_ps(_mm_mul_ps(VecSwizzle(vec1, 3, 3, 0, 0), vec2),
			_mm_mul_ps(VecSwizzle(vec1, 1, 1, 2, 2), VecSwizzle(vec2, 2, 3, 0, 1)));
	}
	// 2x2 row major Matrix multiply adjugate A*(B#)
	__forceinline __m128 Mat2MulAdj(__m128 vec1, __m128 vec2)
	{
		return
			_mm_sub_ps(_mm_mul_ps(vec1, VecSwizzle(vec2, 3, 0, 3, 0)),
			_mm_mul_ps(VecSwizzle(vec1, 1, 0, 3, 2), VecSwizzle(vec2, 2, 1, 2, 1)));
	}

