#include <string>
#include <tge/engine.h>
#include <tge/error/ErrorManager.h>
#include <tge/drawers/SpriteDrawer.h>
#include <tge/graphics/Camera.h>
#include <tge/graphics/GraphicsEngine.h>
#include <tge/sprite/sprite.h>
#include <tge/engine.h>
#include <tge/primitives/CustomShape.h>
#include <tge/shaders/SpriteShader.h>
#include <tge/texture/TextureManager.h>
#include "../../TutorialCommon/TutorialCommon.h"
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
		TutorialCommon::Init(L"TGE: Tutorial 10 - Shaders from shadertoy.com", true);

		// Create the sprite WITHOUT a image!
		Tga::SpriteSharedData sprite1Shared = {}, sprite2Shared = {}, sprite3Shared = {};
		Tga::Sprite2DInstanceData sprite1Instance = {}, sprite2Instance = {}, sprite3Instance = {};

		// Setting the pivot so all operations will be in the middle of the image (rotation, position, etc.)
		sprite1Instance.myPivot = Tga::Vector2f(0.5f, 0.5f);
		sprite2Instance.myPivot = Tga::Vector2f(0.5f, 0.5f);
		sprite3Instance.myPivot = Tga::Vector2f(0.5f, 0.5f);

		/////////// Custom shader

		// Create a new shader
		Tga::SpriteShader customShader; // Create
		customShader.SetShaderdataFloat4(Tga::Vector4f(1, 0, 0, 1), Tga::ShaderDataID_1); // Add some data to it
		customShader.SetTextureAtRegister(Tga::Engine::GetInstance()->GetTextureManager().GetTexture(L"sprites/tge_logo_w.dds"), Tga::ShaderTextureSlot_1); // Add a texture

		// Run PostInit to set all the data
		customShader.Init(L"shaders/instanced_sprite_shader_vs.cso", L"tutorial10/shaders/custom_sprite_pixel_shader_ps.cso");

		// Tell the sprite to use this shader
		sprite1Shared.myCustomShader = &customShader;
		sprite1Instance.myPosition = Tga::Vector2f(0.5f, 0.4f);
		sprite1Instance.mySize = { 0.3f, 0.3f };


		// Second sprite
		Tga::SpriteShader customShader2; // Create
		customShader2.SetShaderdataFloat4(Tga::Vector4f(1, 0, 1, 1), Tga::ShaderDataID_1); // Add some data to it
		customShader2.SetTextureAtRegister(Tga::Engine::GetInstance()->GetTextureManager().GetTexture(L"sprites/tge_logo_w.dds"), Tga::ShaderTextureSlot_1); // Add a texture																																				   // Run PostInit to set all the data
		customShader2.Init(L"shaders/instanced_sprite_shader_vs.cso", L"tutorial10/shaders/custom_sprite_pixel_shader_2_ps.cso");

		// Tell the sprite to use this shader
		sprite2Shared.myCustomShader = &customShader2;
		sprite2Instance.myPosition = Tga::Vector2f(0.5f, 0.5f);
		sprite2Instance.mySize = { 0.3f, 0.3f };

		// Third
		Tga::SpriteShader customShader3; // Create
		customShader3.SetShaderdataFloat4(Tga::Vector4f(1, 0, 1, 1), Tga::ShaderDataID_1); // Add some data to it
		customShader3.SetTextureAtRegister(Tga::Engine::GetInstance()->GetTextureManager().GetTexture(L"sprites/tge_logo_w.dds"), Tga::ShaderTextureSlot_1); // Add a texture																																				   // Run PostInit to set all the data
		customShader3.Init(L"shaders/instanced_sprite_shader_vs.cso", L"tutorial10/shaders/custom_sprite_pixel_shader_3_ps.cso");

		// Tell the sprite to use this shader
		sprite3Shared.myCustomShader = &customShader3;
		sprite3Instance.myPosition = Tga::Vector2f(0.5f, 0.6f);
		sprite3Instance.mySize = { 0.3f, 0.3f };

		float timer = 0;
		while (true)
		{
			timer += 1.0f / 60.0f;
			if (!Tga::Engine::GetInstance()->BeginFrame())
			{
				break;
			}

			{
				Tga::Camera camera;
				float ratio = Tga::Engine::GetInstance()->GetWindowRatioInversed();
				camera.SetOrtographicProjection(0.f, 1.f, 0.5f - 0.5f * ratio, 0.5f + 0.5f * ratio, 0.f, 1.f);
				Tga::Engine::GetInstance()->GetGraphicsEngine().SetCamera(camera);
			}

			Tga::SpriteDrawer& spriteDrawer = Tga::Engine::GetInstance()->GetGraphicsEngine().GetSpriteDrawer();

			spriteDrawer.Draw(sprite1Shared, sprite1Instance);
			spriteDrawer.Draw(sprite2Shared, sprite2Instance);
			spriteDrawer.Draw(sprite3Shared, sprite3Instance);

			Tga::Engine::GetInstance()->EndFrame();
		}
	}

	Tga::Engine::GetInstance()->Shutdown();
}
