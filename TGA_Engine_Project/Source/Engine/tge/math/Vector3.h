#pragma once
#include <array>
#include <cmath>
#include <assert.h>
#include <istream>
#include <tuple>
#include "Vector2.h"

namespace Tga
{
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
			struct { T x; T y; T z; };
			struct { T X; T Y; T Z; };
			struct { T Roll; T Pitch; T Yaw; };
		};

		static const Vector3<T> Zero;
		static const Vector3<T> One;
		static const Vector3<T> Up;
		static const Vector3<T> Forward;
		static const Vector3<T> Right;

		Vector3<T>();
		Vector3<T>(const T& aX, const T& aY, const T& aZ);
		Vector3<T>(const T& aScalar);
		Vector3<T>(const Vector3<T>& aVector3) = default;
		Vector3<T>(Vector3<T>&& aVector3) = default;
		Vector3<T>& operator=(const Vector3<T>& aVector3) = default;
		Vector3<T>& operator=(Vector3<T>&& aVector3) = default;
		//Creates a vector (aX, aY, aZ)
		Vector3<T>(const Vector4<T>& aVector4);
		Vector3<T>(const Vector2<T>& aXY, const T& aZ);
		Vector3<T>(std::array<float, 3> aFloatArray);

		T LengthSqr() const;
		T Length() const;

		Vector3<T> GetNormalized() const;
		void Normalize();

		T Dot(const Vector3<T>& aVector) const;
		Vector3<T> Cross(const Vector3<T>& aVector) const;

		static Vector3<T> Abs(const Vector3<T>& aVector);
		static T Distance(const Vector3<T>& aVector0, const Vector3<T>& aVector1);

		static Vector3<T> Lerp(const Vector3<T>& aStart, const Vector3<T>& aEnd, const float aPercent);

		static Vector3<T> NLerp(const Vector3<T>& aStart, const Vector3<T>& aEnd, const float aPercent);

		//static Vector3<T> Zero();
		//static Vector3<T> One();
		//static Vector3<T> Forward( );
		//static Vector3<T> Right( );
		//static Vector3<T> Up( );
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
	inline Vector3<T>::Vector3() : Vector3(0, 0, 0)
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

	template <class T>
	inline Vector3<T>::Vector3(std::array<float, 3> aFloatArray)
	{
		assert(aFloatArray.size() == 3);
		X = aFloatArray[0];
		Y = aFloatArray[1];
		Z = aFloatArray[2];
	}

	template<class T>
	inline T Vector3<T>::LengthSqr() const
	{
		return abs((X * X) + (Y * Y) + (Z * Z));
	}

	template<class T>
	inline T Vector3<T>::Length() const
	{
		return abs(sqrt((X * X) + (Y * Y) + (Z * Z)));
	}

	template<class T>
	inline Vector3<T> Vector3<T>::GetNormalized() const
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
	inline void Vector3<T>::Normalize()
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
		return direction.Length();
	}
	template<class T>
	inline Vector3<T> Vector3<T>::Lerp(const Vector3<T>& aStart, const Vector3<T>& aEnd, const float aPercent)
	{
		return (aStart + aPercent * (aEnd - aStart));
	}
	template<class T>
	inline Vector3<T> Vector3<T>::NLerp(const Vector3<T>& aStart, const Vector3<T>& aEnd, const float aPercent)
	{
		return Lerp(aStart, aEnd, aPercent).Normalize();
	}

	//template <class T>
	//inline Vector3<T> Vector3<T>::Zero()
	//{
	//	return { 0, 0, 0 };
	//}

	//template <class T>
	//Vector3<T> Vector3<T>::One()
	//{
	//	return { 1, 1, 1 };
	//}

	//template<class T>
	//inline Vector3<T> Vector3<T>::Forward( )
	//{
	//	return { 0, 0, 1 };
	//}
	//template<class T>
	//inline Vector3<T> Vector3<T>::Right( )
	//{
	//	return  { 1, 0, 0 };
	//}
	//template<class T>
	//inline Vector3<T> Vector3<T>::Up( )
	//{
	//	return { 0, 1, 0 };
	//}
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

} // namespace Tga