#pragma once

#include "shader.h"
#include "ShaderCommon.h"
#include <tge/animation/Animation.h>
#include <tge/math/CommonMath.h>
#include <tge/math/matrix4x4.h>
#include <tge/model/model.h>

namespace Tga
{
	class RenderObjectSprite;
	class ModelShader : public Shader
	{
	public:
		ModelShader(Engine* aEngine);
		~ModelShader();

		bool Init() override;
		bool Init(const wchar_t* aVertexShaderFile, const wchar_t* aPixelShaderFile);
		void Render(const RenderObjectSharedData& sharedData, const TextureResource* const* someTextures, const Model::MeshData& aModelData, const Matrix4x4f& aObToWorld, const Matrix4x4f* someBones = nullptr) const;
		bool CreateInputLayout(const std::string& aVS) override;
	private:
		struct ID3D11Buffer* myBoneBuffer;
		struct ID3D11Buffer* myObjectBuffer;
	};
} // namespace Tga