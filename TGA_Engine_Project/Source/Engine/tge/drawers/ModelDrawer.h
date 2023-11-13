#pragma once

#include <memory>
#include <tge/render/RenderCommon.h>
#include <tge/render/RenderObject.h>
#include <tge/shaders/ShaderCommon.h>
#include <wrl\client.h>

struct ID3D11Buffer;
using Microsoft::WRL::ComPtr;

namespace Tga
{
	class ModelInstancer;
	class InstancedModelShader;
	class AnimatedModelInstance;
	class ModelInstance;
	class ModelShader;

	class ModelDrawer
	{
	public:
		ModelDrawer();
		~ModelDrawer();
		bool Init();

		void Draw(const AnimatedModelInstance& modelInstance);
		void Draw(const ModelInstance& modelInstance);
		void DrawPbr(const AnimatedModelInstance& modelInstance);
		void DrawPbr(const ModelInstance& modelInstance);
		void Draw(const AnimatedModelInstance& modelInstance, const ModelShader& shader);
		void Draw(const ModelInstance& modelInstance, const ModelShader& shader);

		void Draw(const ModelInstancer& modelInstancer);

	private:

		std::unique_ptr<ModelShader> myDefaultShader;
		std::unique_ptr<ModelShader> myDefaultAnimatedModelShader;
		std::unique_ptr<ModelShader> myPbrShader;
		std::unique_ptr<ModelShader> myPbrAnimatedModelShader;
		std::unique_ptr<InstancedModelShader> myDefaultInstancedModelShader;

		bool myIsLoaded = false;
		bool myIsInBatch = false;
	};
}
