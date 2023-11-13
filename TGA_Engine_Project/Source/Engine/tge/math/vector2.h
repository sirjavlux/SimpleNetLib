#pragma once
#include <cmath>
#include <iostream>
#ifdef _DEBUG
#include <cassert>
#endif // _DEBUG


namespace Tga
{
	template <class T>
	class Vector2
	{
	public:
		union
		{
			T x;
			T X;
			T myX;
		};
		union
		{
			T y;
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

		T Cross(const Vector2& aVector) const;

		T Dot(const Vector2<T>& aVector) const;

		void Set(const T& aX, const T& aY);

		Vector2 operator/(const T aType) const;

		template <class U>
		friend std::ostream& operator<<(std::ostream& os, const Vector2<U>& aVector);

		static Vector2<T> Abs(const Vector2<T>& aVector);
		static T Distance(const Vector2<T>& aVector0, const Vector2<T>& aVector1);
		static T DistanceSqr(const Vector2<T>& aVector0, const Vector2<T>& aVector1);
		static Vector2<T> Lerp(const Vector2<T>& aStart, const Vector2<T>& aEnd, const float aPercent);
		static Vector2<T> NLerp(const Vector2<T>& aStart, const Vector2<T>& aEnd, const float aPercent);
	};

	typedef Vector2<float> Vector2f;
	typedef Vector2<unsigned int>  Vector2ui;
	typedef Vector2<int>  Vector2i;

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

	template<typename T>
	T Vector2<T>::Cross(const Vector2& aVector) const
	{
		return (myX * aVector.myY) - (myY * aVector.myX);
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
	Vector2<T> Vector2<T>::operator/(const T aType) const
	{
		Vector2<T> vec;
		vec.myX = myX / aType;
		vec.myY = myY / aType;
		return vec;
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

#pragma region Static Functions

	template<class T>
	inline Vector2<T> Vector2<T>::Abs(const Vector2<T>& aVector)
	{
		return { std::abs(aVector.X), std::abs(aVector.Y), std::abs(aVector.Z) };
	}

	template <class T>
	inline T Vector2<T>::Distance(const Vector2<T>& aVector0, const Vector2<T>& aVector1)
	{
		const Vector2<T> direction = aVector1 - aVector0;
		return direction.Length();
	}

	template <class T>
	inline T Vector2<T>::DistanceSqr(const Vector2<T>& aVector0, const Vector2<T>& aVector1)
	{
		const Vector2<T> direction = aVector1 - aVector0;
		return direction.LengthSqr();
	}

	template<class T>
	inline Vector2<T> Vector2<T>::Lerp(const Vector2<T>& aStart, const Vector2<T>& aEnd, const float aPercent)
	{
		return (aStart + aPercent * (aEnd - aStart));
	}
	template<class T>
	inline Vector2<T> Vector2<T>::NLerp(const Vector2<T>& aStart, const Vector2<T>& aEnd, const float aPercent)
	{
		return Lerp(aStart, aEnd, aPercent).Normalize();
	}

#pragma endregion Static Functions
} // namespace Tga
