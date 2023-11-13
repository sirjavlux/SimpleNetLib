#pragma once
#include "Vector4.h"

namespace Tga
{
	class Color
	{
	public:
		Color()
			:myR(0.0f)
			, myG(0.0f)
			, myB(0.0f)
			, myA(0.0f)
		{}

		Color(float aR, float aG, float aB)
		{
			myR = aR;
			myG = aG;
			myB = aB;
			myA = 1.0;
		}

		Color(float aR, float aG, float aB, float aA)
		{
			myR = aR;
			myG = aG;
			myB = aB;
			myA = aA;
		}

		void Set(float aR, float aG, float aB, float aA)
		{
			myR = aR;
			myG = aG;
			myB = aB;
			myA = aA;
		}

		bool operator==(const Color& other) const {
			return (myR == other.myR && myG == other.myG && myB == other.myB && myA == other.myA);
		}

		Vector4f AsVec4() const
		{
			return Vector4f(myR, myG, myB, myA);
		}

		static float InverseEOTF(float x)
		{
			if (x >= 0.04045f)
				return powf((x + 0.055f) / (1.0f + 0.055f), 2.4f);
			else
				return x / 12.92f;
		}

		Vector4f AsLinearVec4() const
		{
			return Vector4f(InverseEOTF(myR), InverseEOTF(myG), InverseEOTF(myB), myA);
		}

		unsigned int AsHex() const
		{
			unsigned char r = static_cast<unsigned char>(myR * 255.0f);
			unsigned char g = static_cast<unsigned char>(myG * 255.0f);
			unsigned char b = static_cast<unsigned char>(myB * 255.0f);
			unsigned char a = static_cast<unsigned char>(myA * 255.0f);

			unsigned int final = 0;
			final |= (a << 24);
			final |= (r << 16);
			final |= (g << 8);
			final |= (b);
			return final;
		}

		float myR;
		float myG;
		float myB;
		float myA;
	};
}