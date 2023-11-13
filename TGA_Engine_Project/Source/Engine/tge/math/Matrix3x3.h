#pragma once
#include "Matrix4x4.h"
#include <assert.h>
#include <cmath>
#include "Vector3.h"

namespace Tga
{
	template<typename T>
	class Matrix3x3
	{
	public:
		Matrix3x3<T>();
		Matrix3x3<T>(const Matrix3x3<T>& aMatrix);
		Matrix3x3<T>(const Matrix4x4<T>& aMatrix);
		Matrix3x3<T>(std::initializer_list<T> aList);

		Matrix3x3<T>& operator=(const Matrix3x3<T>& aMatrix);

		T& operator()(const int aRow, const int aColumn);
		const T& operator()(const int aRow, const int aColumn) const;
		bool operator==(const Matrix3x3<T>& aMatrix) const;
		bool operator!=(const Matrix3x3<T>& aMatrix) const;

		Matrix3x3<T> operator+(const Matrix3x3<T>& aMatrix) const;
		Matrix3x3<T> operator-(const Matrix3x3<T>& aMatrix) const;
		Matrix3x3<T> operator*(const Matrix3x3<T>& aMatrix) const;
		Matrix3x3<T> operator*(const T& aScalar) const;
		Vector3<T> operator*(const Vector3<T>& aVector) const;
		Matrix3x3<T>& operator+=(const Matrix3x3<T>& aMatrix);
		Matrix3x3<T>& operator-=(const Matrix3x3<T>& aMatrix);
		Matrix3x3<T>& operator*=(const Matrix3x3<T>& aMatrix);
		Matrix3x3<T>& operator*=(const T& aScalar);

		static Matrix3x3<T> CreateRotationAroundX(const T aAngleInRadians);
		static Matrix3x3<T> CreateRotationAroundY(const T aAngleInRadians);
		static Matrix3x3<T> CreateRotationAroundZ(const T aAngleInRadians);
		static Matrix3x3<T> Transpose(const Matrix3x3<T>& aMatrixToTranspose);

	private:
		static const size_t myLength = 9;
		T myData[myLength];
	};

	typedef Matrix3x3<float> Matrix3x3f;

	template <typename T> Vector3<T> operator*(const Vector3<T>& aVector, const Matrix3x3<T> aMatrix)
	{
		Vector3<T> result;
		result.X = (aMatrix(1, 1) * aVector.X) + (aMatrix(2, 1) * aVector.Y) + (aMatrix(3, 1) * aVector.Z);
		result.Y = (aMatrix(1, 2) * aVector.X) + (aMatrix(2, 2) * aVector.Y) + (aMatrix(3, 2) * aVector.Z);
		result.Z = (aMatrix(1, 3) * aVector.X) + (aMatrix(2, 3) * aVector.Y) + (aMatrix(3, 3) * aVector.Z);
		return result;
	};

#pragma region Constructors
	template <typename T> Matrix3x3<T>::Matrix3x3() : myData()
	{
		std::memset(myData, 0, myLength * sizeof(T));
		myData[0] = 1;
		myData[4] = 1;
		myData[8] = 1;
	}
	template <typename T> Matrix3x3<T>::Matrix3x3(const Matrix3x3<T>& aMatrix)
	{
		memcpy(myData, aMatrix.myData, sizeof(T) * myLength);
	}

	template<typename T> inline Matrix3x3<T>::Matrix3x3(std::initializer_list<T> aList)
	{
		assert(aList.size() <= myLength && "Initializer list contains too many values.");
		std::memcpy(myData, aList.begin(), sizeof(T) * aList.size());
		if (aList.size() < myLength)
		{
			std::fill(myData + aList.size(), myData + myLength, myData[aList.size() - 1]);
		}
	}

	template<typename T>
	inline Matrix3x3<T>::Matrix3x3(const Matrix4x4<T>& aMatrix)
	{
		const size_t rowLength = sizeof(T) * 3;
		std::memcpy(myData, &aMatrix(1, 1), rowLength);
		std::memcpy(myData + 3, &aMatrix(2, 1), rowLength);
		std::memcpy(myData + 6, &aMatrix(3, 1), rowLength);
	}
#pragma endregion Constructors

#pragma region Operators

	template<typename T>
	inline Matrix3x3<T>& Matrix3x3<T>::operator=(const Matrix3x3<T>& aMatrix)
	{
		std::memcpy(myData, aMatrix.myData, sizeof(T) * myLength);
		return *this;
	}

	// Rows and Columns start at 1.
	template<typename T> inline T& Matrix3x3<T>::operator()(const int aRow, const int aColumn)
	{
		assert(aRow > 0 && aRow < 4 && aColumn > 0 && aColumn < 4 && "Argument out of bounds");
		return myData[(aRow - 1) * 3 + (aColumn - 1)];
	}

	// Rows and Columns start at 1.
	template<typename T> inline const T& Matrix3x3<T>::operator()(const int aRow, const int aColumn) const
	{
		assert(aRow > 0 && aRow < 4 && aColumn > 0 && aColumn < 4 && "Argument out of bounds");
		return myData[(aRow - 1) * 3 + (aColumn - 1)];
	}

	template<typename T>
	inline bool Matrix3x3<T>::operator==(const Matrix3x3<T>& aMatrix) const
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
	inline bool Matrix3x3<T>::operator!=(const Matrix3x3<T>& aMatrix) const
	{
		return !operator==(aMatrix);
	}

	template<typename T>
	inline Matrix3x3<T> Matrix3x3<T>::operator+(const Matrix3x3<T>& aMatrix) const
	{
		Matrix3x3<T> result{ *this };
		return result += aMatrix;
	}

	template<typename T>
	inline Matrix3x3<T>& Matrix3x3<T>::operator+=(const Matrix3x3<T>& aMatrix)
	{
		for (auto i = 0; i < myLength; i++)
		{
			myData[i] += aMatrix.myData[i];
		}
		return *this;
	}

	template<typename T>
	inline Matrix3x3<T> Matrix3x3<T>::operator-(const Matrix3x3<T>& aMatrix) const
	{
		Matrix3x3<T> result{ *this };
		return result -= aMatrix;
	}

	template<typename T>
	inline Matrix3x3<T>& Matrix3x3<T>::operator-=(const Matrix3x3<T>& aMatrix)
	{
		for (auto i = 0; i < myLength; i++)
		{
			myData[i] -= aMatrix.myData[i];
		}
		return *this;
	}

	template<typename T>
	inline Matrix3x3<T> Matrix3x3<T>::operator*(const Matrix3x3<T>& aMatrix) const
	{
		Matrix3x3<T> result{ *this };
		return result *= aMatrix;
	}

	template<typename T>
	inline Matrix3x3<T>& Matrix3x3<T>::operator*=(const Matrix3x3<T>& aMatrix)
	{
		Matrix3x3<T> result;
		for (int i = 1; i <= 3; i++)
		{
			for (auto j = 1; j <= 3; j++)
			{
				T product{ 0 };
				for (auto k = 1; k <= 3; k++)
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
	inline Matrix3x3<T> Matrix3x3<T>::operator*(const T& aScalar) const
	{
		Matrix3x3<T> result{ *this };
		return result *= aScalar;
	}
	template<typename T>
	inline Vector3<T> Matrix3x3<T>::operator*(const Vector3<T>& aVector) const
	{
		Vector3<T> result;
		result.X = (myData[0] * aVector.X) + (myData[3] * aVector.Y) + (myData[6] * aVector.Z);
		result.Y = (myData[1] * aVector.X) + (myData[4] * aVector.Y) + (myData[7] * aVector.Z);
		result.Z = (myData[2] * aVector.X) + (myData[5] * aVector.Y) + (myData[8] * aVector.Z);
		return result;
	}
	template<typename T>
	inline Matrix3x3<T>& Matrix3x3<T>::operator*=(const T& aScalar)
	{
		for (int i = 0; i < myLength; i++)
		{
			myData[i] *= aScalar;
		}
		return *this;
	}

#pragma endregion Operators

#pragma region Static Functions

	template<typename T>
	inline Matrix3x3<T> Matrix3x3<T>::CreateRotationAroundX(const T aAngleInRadians)
	{
		const T cos = std::cos(aAngleInRadians);
		const T sin = std::sin(aAngleInRadians);
		return
		{
			T(1), 0, 0,
			0, cos, sin,
			0, -sin, cos
		};
	}

	template<typename T>
	inline Matrix3x3<T> Matrix3x3<T>::CreateRotationAroundY(const T aAngleInRadians)
	{
		const T cos = std::cos(aAngleInRadians);
		const T sin = std::sin(aAngleInRadians);
		return
		{
			cos, 0, -sin,
			0, T(1), 0,
			sin, 0, cos
		};
	}

	template<typename T>
	inline Matrix3x3<T> Matrix3x3<T>::CreateRotationAroundZ(const T aAngleInRadians)
	{
		const T cos = std::cos(aAngleInRadians);
		const T sin = std::sin(aAngleInRadians);
		return
		{
			cos,  sin, 0,
			-sin, cos, 0,
			0,    0, T(1)
		};
	}

	template<typename T>
	inline Matrix3x3<T> Matrix3x3<T>::Transpose(const Matrix3x3<T>& aMatrixToTranspose)
	{
		Matrix3x3<T> result;
		for (int i = 0; i < 3; i++)
		{
			for (int j = 0; j < 3; j++)
			{
				result.myData[i + j * 3] = aMatrixToTranspose.myData[j + i * 3];
			}
		}
		return result;
	}

#pragma endregion Static Functions
} // namespace Tga