#include <string>

#include "../../TutorialCommon/TutorialCommon.h"
#include <tge/drawers/CustomShapeDrawer.h>
#include <tge/drawers/SpriteDrawer.h>
#include <tge/graphics/GraphicsEngine.h>
#include <tge/sprite/sprite.h>
#include <tge/engine.h>
#include <tge/light/LightManager.h>
#include <tge/graphics/Camera.h>
#include <tge/graphics/DX11.h>
#include <tge/primitives/CustomShape.h>
#include <tge/shaders/SpriteShader.h>
#include <tge/texture/TextureManager.h>
#include <tge/settings/settings.h>

// Making sure that DX2DEngine lib is linked
//
#ifdef _DEBUG
#pragma comment(lib,"Engine_Debug.lib")
#pragma comment(lib,"External_Debug.lib")
#else
#pragma comment(lib,"Engine_Release.lib")
#pragma comment(lib,"External_Release.lib")
#endif // _DEBUG


void Go( void );
int main( const int /*argc*/, const char * /*argc*/[] )
{
    Go();

    return 0;
}

void Go( void )
{
	Tga::LoadSettings(TGE_PROJECT_SETTINGS_FILE);
	{
		TutorialCommon::Init(L"TGE: Tutorial 7");

		// Background, not needed, but beautiful
		Tga::CustomShape2D myShape;
		myShape.Reset();
		myShape.AddPoint({ 0.0f, 0.0f }, Tga::Color(1, 0, 0, 1));
		myShape.AddPoint({ 1.0f, 0.0f }, Tga::Color(0, 1, 0, 1));
		myShape.AddPoint({ 0.0f, 1.0f }, Tga::Color(0, 0, 1, 1));

		myShape.AddPoint({ 1.0f, 0.0f }, Tga::Color(0, 1, 0, 1));
		myShape.AddPoint({ 1.0f, 1.0f }, Tga::Color(0, 0, 1, 1));
		myShape.AddPoint({ 0.0f, 1.0f }, Tga::Color(0, 0, 1, 1));
		myShape.BuildShape();

		Tga::Engine& engine = *Tga::Engine::GetInstance();

		//The target texture we will render to instead of the screen, this is a sprite, which means it have all the nice features of one (rotation, position etc.)
		Tga::RenderTarget myRenderTargetTexture = Tga::RenderTarget::Create({1024,1024});
		Tga::RenderTarget& backBuffer = *Tga::DX11::BackBuffer;

		// Ordinary sprite that we will render onto the target
		Tga::SpriteSharedData logoSpriteSharedData = {};
		Tga::Texture* texture = engine.GetTextureManager().GetTexture(L"Sprites/tge_logo_b.dds");
		logoSpriteSharedData.myTexture = texture;
		Tga::Vector2f logoSize = texture->mySize;

		// We need a couple of tga loggos!
		const int numberOfLoggos = 20;
		std::vector<Tga::Sprite2DInstanceData> logoInstances;
		for (int i = 0; i < numberOfLoggos; i++)
		{
			float randX = (float)(rand() % 100) * 0.01f;
			float randY = (float)(rand() % 100) * 0.01f;
			Tga::Sprite2DInstanceData logo;
			logo.myPosition = { randX , randY };
			logo.myPivot = { 0.5f, 0.5f };
			logo.mySize = { 0.5f * logoSize.myX, 0.5f * logoSize.myY };

			logoInstances.push_back(logo);
		}

		// Create a new shader to showcase the fullscreen shader with.
		Tga::SpriteShader customShader; // Create	
		customShader.Init(L"shaders/instanced_sprite_shader_vs.cso", L"Tutorial7/shaders/custom_sprite_pixel_shader_PS.cso");


		float timer = 0;
		while (true)
		{
			timer += 1.0f / 60.0f;
			if (!Tga::Engine::GetInstance()->BeginFrame())
			{
				break;
			}

			Tga::Vector2ui intResolution = Tga::Engine::GetInstance()->GetRenderSize();
			Tga::Vector2f resolution = { (float)intResolution.x, (float)intResolution.y };

			// Render background
			myShape.SetSize(resolution);
			Tga::Engine::GetInstance()->GetGraphicsEngine().GetCustomShapeDrawer().Draw(myShape);

			// Set the new sprite as a target instead of the screen
			myRenderTargetTexture.SetAsActiveTarget();
			myRenderTargetTexture.Clear();

			// set a custom camera with square aspect ratio:
			Tga::Camera camera;
			camera.SetOrtographicProjection(0.f, 1.f, 0.f, 1.f, 0.f, 1.f);
			engine.GetGraphicsEngine().SetCamera(camera);

			// Render all the logos onto the sprite
			engine.GetGraphicsEngine().GetSpriteDrawer().Draw(logoSpriteSharedData, logoInstances.data(), logoInstances.size());

			// Set the target back to the screen
			backBuffer.SetAsActiveTarget();
			engine.GetGraphicsEngine().ResetToDefaultCamera();

			// Render the target sprite which holds a lot of logos!
			{
				Tga::SpriteSharedData sharedData = {};
				sharedData.myTexture = &myRenderTargetTexture;
				sharedData.myCustomShader = &customShader;

				Tga::Sprite2DInstanceData instanceData = {};
				instanceData.myPosition = Tga::Vector2f(0.5f, 0.5f) * resolution;
				instanceData.myPivot = Tga::Vector2f(0.5f, 0.5f);
				instanceData.mySize = Tga::Vector2f(0.7f, 0.7f) * resolution.y;
				instanceData.myRotation = cosf(timer) * 0.1f;

				engine.GetGraphicsEngine().GetSpriteDrawer().Draw(sharedData, instanceData);
			}

			Tga::Engine::GetInstance()->EndFrame();
		}
	}

	Tga::Engine::GetInstance()->Shutdown();
}
