/*
Use this class to create and show a sprite
*/

#pragma once
#include <tge/render/RenderCommon.h>
#include <tge/math/Color.h>
#include <tge/math/Matrix4x4.h>

namespace Tga
{
	class TextureResource;
	class SpriteShader;

	struct SpriteSharedData : RenderObjectSharedData
	{
		const TextureResource* myTexture = nullptr;
		TextureResource* myMaps[MAP_MAX] = { nullptr };
		SpriteShader* myCustomShader = nullptr;
	};

	struct Sprite2DInstanceData
	{
		Vector2f myPosition = { 0.0f, 0.0f };
		Vector2f myPivot = { 0.5f, 0.5f };
		Vector2f mySize = { 1.0f, 1.0f };
		Vector2f mySizeMultiplier = { 1.0f, 1.0f };
		Vector2f myUV = { 0.0f, 0.0f };
		Vector2f myUVScale = { 1.0f, 1.0f };
		Color myColor = { 1.0f, 1.0f, 1.0f, 1.0f };
		TextureRext myTextureRect = { 0.0f, 0.0f, 1.0f, 1.0f };
		float myRotation = 0.f;
		bool myIsHidden = false;
	};

	struct Sprite3DInstanceData
	{
		Matrix4x4f myTransform = {};
		Vector2f myUV = { 0.0f, 0.0f };
		Vector2f myUVScale = { 1.0f, 1.0f };
		Color myColor = { 1.0f, 1.0f, 1.0f, 1.0f };
		TextureRext myTextureRect = { 0.0f, 0.0f, 1.0f, 1.0f };
		bool myIsHidden = false;
	};
}