#include "stdafx.h"
#include "FMath.h"
#include "Vector3.h"
#include <math.h>

namespace FMath
{
	
	int SzudzikPairingFunction(int aValue0, int aValue1)
	{
		// http://szudzik.com/ElegantPairing.pdf
		// This is a more space efficent version of Cantors pairing function and also accounts for negative numbers
		int a = aValue0 >= 0 ? 2 * aValue0 : -2 * aValue0 - 1;
		int b = aValue1 >= 0 ? 2 * aValue1 : -2 * aValue1 - 1;
		int value = a >= b ? a * a + a + b : a + b * b;
		assert(value >= 0 && "Pairing value was negative. Most likely due to overflow from big numbers. Consider using long instead of int.");
		return value;
	}

	float InverseLerp(const float aFrom, const float aTo, const float aValue)
	{
		return (aValue - aFrom) / (aTo - aFrom);
	}

}