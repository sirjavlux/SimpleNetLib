#pragma once
#include <math.h>
#ifdef _DEBUG
#include <cassert>
#endif // _DEBUG
#include "Vector3.h"
#include "Vector2.h"

#pragma warning (disable : 4201) // Nonstandard nameless struct/union.

namespace Tga
{
	template <class T>
	class Vector4
	{
	public:
		union
		{
			T myValues[4];
			struct { T x; T y; T z; T w; };
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

} // namespace Tga