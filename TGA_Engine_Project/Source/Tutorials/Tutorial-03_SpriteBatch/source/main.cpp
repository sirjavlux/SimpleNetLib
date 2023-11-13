#include <string>
#include <tge/engine.h>
#include <tge/error/ErrorManager.h>

#include <tge/graphics/GraphicsEngine.h>
#include <tge/sprite/sprite.h>
#include <tge/drawers/SpriteDrawer.h>
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



// This is where the application starts of for real. By keeping this in it's own file
// we will have the same behaviour for both console and windows startup of the
// application.
//
void Go( void )
{
	Tga::LoadSettings(TGE_PROJECT_SETTINGS_FILE);
	{
		TutorialCommon::Init(L"TGE: Tutorial 3");

		Tga::Engine& engine = *Tga::Engine::GetInstance();

		Tga::Vector2ui intResolution = Tga::Engine::GetInstance()->GetRenderSize();
		Tga::Vector2f resolution = { (float)intResolution.x, (float)intResolution.y};

		// Get a sprite drawer, for drawing sprites
		Tga::SpriteDrawer& spriteDrawer(engine.GetGraphicsEngine().GetSpriteDrawer());

		Tga::SpriteSharedData sharedData;
		sharedData.myTexture = Tga::Engine::GetInstance()->GetTextureManager().GetTexture(L"sprites/tge_logo_w.dds");

		std::vector<Tga::Sprite2DInstanceData> instances;
		for (unsigned int i = 0; i < 100000; i++)
		{
			float randomX = static_cast<float>(rand() % 1000) / 1000.0f;
			float randomY = static_cast<float>(rand() % 1000) / 1000.0f;

			Tga::Sprite2DInstanceData instance = {};

			// Random position on screen
			instance.myPosition = resolution*Tga::Vector2f(randomX, randomY);
			// Color based on its location on screen
			instance.myColor = Tga::Color(randomX, randomY, randomX, 1);
			// Size is 10% of screen
			instance.mySize = Tga::Vector2f(0.1f, 0.1f) * resolution.y;
			instances.push_back(instance);
		}

		// MAIN LOOP
		while (true)
		{
			if (!Tga::Engine::GetInstance()->BeginFrame())
			{
				break;
			}

			spriteDrawer.Draw(sharedData, &instances[0], instances.size());

			Tga::Engine::GetInstance()->EndFrame();
		}
	}

	Tga::Engine::GetInstance()->Shutdown();
}
