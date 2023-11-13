#pragma once
#include <tge/math/vector2.h>
#include <tge/math/color.h>
#include <tge/math/matrix4x4.h>

#define SPRITE_BATCH_COUNT 1024
namespace Tga
{
	enum class ConstantBufferSlot
	{
		Common = 0,
		Light = 1,
		Object = 2,
		Bones = 3,
		Count,
	};
	struct SpriteShaderInstanceData
	{
		Matrix4x4f myTransform;
		Vector4f myColor;
		Vector4f myUV;
		Vector4f myUVRect;
	};

}