#pragma once
#include <array>
#include <memory>
#include <thread>
#include <wrl/client.h>

#include "FullscreenPixelateEffect.h"
using Microsoft::WRL::ComPtr;

struct ID3D11Buffer;

namespace Tga
{ 
	class Engine;
	class RenderObject;
	class SpriteDrawer;
	class LineDrawer;
	class CustomShapeDrawer;
	class ModelDrawer;
	class RenderObjectCustom;
	class Camera;
	class FullscreenEffect;
	class ParticleRenderer;

	class GraphicsEngine
	{
	public:
		GraphicsEngine(void);
		~GraphicsEngine(void);
		bool Init();
		bool IsInitiated();

		SpriteDrawer& GetSpriteDrawer() { return *mySpriteDrawer; };
		ModelDrawer& GetModelDrawer() { return *myModelDrawer; };
		CustomShapeDrawer& GetCustomShapeDrawer() { return *myCustomShapeDrawer; };
		LineDrawer& GetLineDrawer() { return *myLineDrawer; };

		FullscreenEffect& GetFullscreenEffectCopy() { return *myFullscreenCopy; };
		FullscreenEffect& GetFullscreenEffectTonemap() { return *myFullscreenTonemap; };
		FullscreenEffect& GetFullscreenEffectVerticalGaussianBlur() { return *myFullscreenVerticalGaussianBlur; };
		FullscreenEffect& GetFullscreenEffectHorizontalGaussianBlur() { return *myFullscreenHorizontalGaussianBlur; };
		FullscreenPixelateEffect& GetFullscreenEffectPixelation() { return *myFullscreenPixelateEffect; };
		FullscreenEffect& GetFullscreenEffectBanding() { return *myFullscreenBandingEffect; };

		void UpdateDefaultCamera();
		void ResetToDefaultCamera();
		void SetCamera(const Camera& camera);
		const Camera& GetCamera() const;

		void UpdateAndBindCommonBuffer();
		void UpdateAndBindLightBuffer();

		void SetFullScreen(bool aFullScreen);
	private:

		std::unique_ptr<SpriteDrawer> mySpriteDrawer;
		std::unique_ptr<ModelDrawer> myModelDrawer;
		std::unique_ptr<CustomShapeDrawer> myCustomShapeDrawer;
		std::unique_ptr<LineDrawer> myLineDrawer;

		std::unique_ptr<FullscreenEffect> myFullscreenCopy;
		std::unique_ptr<FullscreenEffect> myFullscreenTonemap;
		std::unique_ptr<FullscreenEffect> myFullscreenVerticalGaussianBlur;
		std::unique_ptr<FullscreenEffect> myFullscreenHorizontalGaussianBlur;
		std::unique_ptr<FullscreenPixelateEffect> myFullscreenPixelateEffect;
		std::unique_ptr<FullscreenEffect> myFullscreenBandingEffect;

		ComPtr<ID3D11Buffer> myLightBuffer;
		ComPtr<ID3D11Buffer> myCommonBuffer;
		std::unique_ptr<Camera> myCamera;
		bool myUseDefaultCamera;

		bool myIsInitiated;
	};
}