#pragma once
#include <assert.h>
#include <cmath>
#include <dvec.h>
#include <initializer_list>
#include "Vector.h"
#include "Quaternion.h"

#pragma warning(disable : 6385) // Buffer underrun warning.
#pragma warning(disable : 26495) // Uninitialized warning.

namespace Tga
{
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

		/*This function will decompose the matrix, if this is done every frame and calculated back into a matrix it will twist
		slightly each frame, likely due to precision errors during conversion */
		void DecomposeMatrix(Vector3f& aPosition, Vector3f& aRotation, Vector3f& aScale) const;

		Matrix4x4<T> GetTranspose( ) const;
		Matrix4x4<T> GetInverse( ) const;
		void SetPosition(const Vector3<T>& aVector3);
		void SetUp(const Vector3<T>& aVector3);
		void SetRight(const Vector3<T>& aVector3);
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

		static Matrix4x4<float> InverseFloat(const Matrix4x4<float>& aMatrixToInverse);
		static Matrix4x4<float> InverseFastFloat(const Matrix4x4<float>& aMatrixToInverse);

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
	inline Matrix4x4<T> Matrix4x4<T>::CreateScaleMatrix(Vector3<T> aScaleVector)
	{
		Matrix4x4<T> result;
		result.myData[0] = aScaleVector.X;
		result.myData[5] = aScaleVector.Y;
		result.myData[10] = aScaleVector.Z;
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
	inline void Matrix4x4<T>::SetRight(const Vector3<T>& aVector3)
	{
		myData[0] = aVector3.X;
		myData[1] = aVector3.Y;
		myData[2] = aVector3.Z;
	}

	template<typename T>
	inline void Matrix4x4<T>::SetUp(const Vector3<T>& aVector3)
	{
		myData[4] = aVector3.X;
		myData[5] = aVector3.Y;
		myData[6] = aVector3.Z;
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

	inline Matrix4x4<float> Matrix4x4<float>::InverseFloat(const Matrix4x4<float>& aMatrixToInverse)
	{
		// use block matrix method
		// A is a matrix, then i(A) or iA means inverse of A, A# (or A_ in code) means adjugate of A, |A| (or detA in code) is determinant, tr(A) is trace

		// sub matrices
		__m128 A = VecShuffle_0101(aMatrixToInverse.m1, aMatrixToInverse.m2);
		__m128 B = VecShuffle_2323(aMatrixToInverse.m1, aMatrixToInverse.m2);
		__m128 C = VecShuffle_0101(aMatrixToInverse.m3, aMatrixToInverse.m4);
		__m128 D = VecShuffle_2323(aMatrixToInverse.m3, aMatrixToInverse.m4);

		__m128 detA = _mm_set1_ps(aMatrixToInverse.m11 * aMatrixToInverse.m22 - aMatrixToInverse.m12 * aMatrixToInverse.m21);
		__m128 detB = _mm_set1_ps(aMatrixToInverse.m13 * aMatrixToInverse.m24 - aMatrixToInverse.m14 * aMatrixToInverse.m23);
		__m128 detC = _mm_set1_ps(aMatrixToInverse.m31 * aMatrixToInverse.m42 - aMatrixToInverse.m32 * aMatrixToInverse.m41);
		__m128 detD = _mm_set1_ps(aMatrixToInverse.m33 * aMatrixToInverse.m44 - aMatrixToInverse.m34 * aMatrixToInverse.m43);

		// let iM = 1/|M| * | X  Y |
		//                  | Z  W |

		// D#C
		__m128 D_C = Mat2AdjMul(D, C);
		// A#B
		__m128 A_B = Mat2AdjMul(A, B);
		// X# = |D|A - B(D#C)
		__m128 X_ = _mm_sub_ps(_mm_mul_ps(detD, A), Mat2Mul(B, D_C));
		// W# = |A|D - C(A#B)
		__m128 W_ = _mm_sub_ps(_mm_mul_ps(detA, D), Mat2Mul(C, A_B));

		// |M| = |A|*|D| + ... (continue later)
		__m128 detM = _mm_mul_ps(detA, detD);

		// Y# = |B|C - D(A#B)#
		__m128 Y_ = _mm_sub_ps(_mm_mul_ps(detB, C), Mat2MulAdj(D, A_B));
		// Z# = |C|B - A(D#C)#
		__m128 Z_ = _mm_sub_ps(_mm_mul_ps(detC, B), Mat2MulAdj(A, D_C));

		// |M| = |A|*|D| + |B|*|C| ... (continue later)
		detM = _mm_add_ps(detM, _mm_mul_ps(detB, detC));

		// tr((A#B)(D#C))
		__m128 tr = _mm_mul_ps(A_B, VecSwizzle(D_C, 0, 2, 1, 3));
		tr = _mm_hadd_ps(tr, tr);
		tr = _mm_hadd_ps(tr, tr);
		// |M| = |A|*|D| + |B|*|C| - tr((A#B)(D#C)
		detM = _mm_sub_ps(detM, tr);

		const __m128 adjSignMask = _mm_setr_ps(1.f, -1.f, -1.f, 1.f);
		// (1/|M|, -1/|M|, -1/|M|, 1/|M|)
		__m128 rDetM = _mm_div_ps(adjSignMask, detM);

		X_ = _mm_mul_ps(X_, rDetM);
		Y_ = _mm_mul_ps(Y_, rDetM);
		Z_ = _mm_mul_ps(Z_, rDetM);
		W_ = _mm_mul_ps(W_, rDetM);

		Matrix4x4<float> r;

		// apply adjugate and store, here we combine adjugate shuffle and store shuffle
		r.m1 = VecShuffle(X_, Y_, 3, 1, 3, 1);
		r.m2 = VecShuffle(X_, Y_, 2, 0, 2, 0);
		r.m3 = VecShuffle(Z_, W_, 3, 1, 3, 1);
		r.m4 = VecShuffle(Z_, W_, 2, 0, 2, 0);

		return r;
	}

	inline Matrix4x4<float> Matrix4x4<float>::InverseFastFloat(const Matrix4x4<float>& aMatrixToInverse)
	{
		Matrix4x4<float> m;

		// transpose 3x3, we know m03 = m13 = m23 = 0
		__m128 t0 = VecShuffle_0101(aMatrixToInverse.m1, aMatrixToInverse.m2); // 00, 01, 10, 11
		__m128 t1 = VecShuffle_2323(aMatrixToInverse.m1, aMatrixToInverse.m2); // 02, 03, 12, 13
		m.m1 = VecShuffle(t0, aMatrixToInverse.m3, 0, 2, 0, 3); // 00, 10, 20, 23(=0)
		m.m2 = VecShuffle(t0, aMatrixToInverse.m3, 1, 3, 1, 3); // 01, 11, 21, 23(=0)
		m.m3 = VecShuffle(t1, aMatrixToInverse.m3, 0, 2, 2, 3); // 02, 12, 22, 23(=0)

																// last line
		m.m4 = _mm_mul_ps(m.m1, VecSwizzle1(aMatrixToInverse.m4, 0));
		m.m4 = _mm_add_ps(m.m4, _mm_mul_ps(m.m2, VecSwizzle1(aMatrixToInverse.m4, 1)));
		m.m4 = _mm_add_ps(m.m4, _mm_mul_ps(m.m3, VecSwizzle1(aMatrixToInverse.m4, 2)));
		m.m4 = _mm_sub_ps(_mm_setr_ps(0.0f, 0.0f, 0.0f, 1.0f), m.m4);

		return m;
	}
} // namespace Tga