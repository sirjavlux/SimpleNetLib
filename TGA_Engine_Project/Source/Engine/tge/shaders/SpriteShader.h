#pragma once
#include <tge/math/matrix4x4.h>
#include <tge/shaders/shader.h>

#define MAX_SHADER_DATA 8
#define MAX_SHADER_TEXTURES 2

using Microsoft::WRL::ComPtr;

namespace Tga
{
	class GraphicsEngine;
	class Engine;
	class TextureResource;
	struct SpriteSharedData;

	enum ShaderDataID
	{
		ShaderDataID_1,
		ShaderDataID_2,
		ShaderDataID_3,
		ShaderDataID_4,
		ShaderDataID_5,
		ShaderDataID_6,
		ShaderDataID_7,
		ShaderDataID_8,
	};
	enum ShaderTextureSlot
	{
		ShaderTextureSlot_1 = 4, // Below 4 is the standard texture slots, keep above it! (: register( t4 ); in the shader)
		ShaderTextureSlot_2 = 5,
		ShaderTextureSlot_3 = 6,
		ShaderTextureSlot_4 = 7,
	};
	enum class ShaderDataBufferIndex
	{
		Index_1 = 5, // Below 5 is the standard buffer slots, keep above it! (: register(b5) in shader)
		Index_2 = 6,
		Index_3 = 7,
		Index_4 = 8,
	};

	class SpriteShader : private Shader
	{
		friend class SpriteDrawer;
	public:
		SpriteShader();
		bool Init(const wchar_t* aVertex = L"shaders/instanced_sprite_shader_VS.cso", const wchar_t* aPixel = L"shaders/sprite_shader_PS.cso");


		void SetDataBufferIndex(ShaderDataBufferIndex aBufferRegisterIndex);
		// Add a float4 to a shader, this must be in the shade file matching 1:1! The Id is the index of the float4 in the shader (top down starting from 0)
		void SetShaderdataFloat4(Tga::Vector4f someData, ShaderDataID aID);

		// Set a texture to the shader, the index: 0, 1, 2, 3 are reserved. Keep the index above this and register it with the same id in the shader
		void SetTextureAtRegister(Tga::TextureResource* aTexture, ShaderTextureSlot aRegisterIndex);

		bool PrepareRender(const SpriteSharedData& aSharedData);

	protected:
		bool CreateInputLayout(const std::string& aVS) override;

	private:
		Tga::Vector4f myCustomData[MAX_SHADER_DATA];
		int myCurrentDataIndex;
		unsigned char myCurrentTextureCount;

		ComPtr<ID3D11Buffer> myCustomBuffer;
		unsigned char myBufferIndex;

		struct BoundTexture
		{
			BoundTexture() {}
			BoundTexture(Tga::TextureResource* aTex, unsigned char aIndex)
			{
				myTexture = aTex;
				myIndex = aIndex;
			}
			Tga::TextureResource* myTexture = nullptr;
			unsigned char myIndex = 0;
		};
		BoundTexture myBoundTextures[MAX_SHADER_TEXTURES];
	};
}