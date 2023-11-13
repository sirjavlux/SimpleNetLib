#include <string>

#include "../../TutorialCommon/TutorialCommon.h"
#include <tge/graphics/GraphicsEngine.h>
#include <tge/graphics/Camera.h>
#include <tge/sprite/sprite.h>
#include <tge/drawers/SpriteDrawer.h>
#include <tge/texture/TextureManager.h>
#include <tge/engine.h>
#include <tge/light/LightManager.h>
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
		TutorialCommon::Init(L"TGE: Tutorial 5");

		Tga::Engine& engine = *Tga::Engine::GetInstance();
		Tga::SpriteDrawer& spriteDrawer(engine.GetGraphicsEngine().GetSpriteDrawer());

		Tga::SpriteSharedData spriteSharedData = {};
		Tga::Texture* texture = engine.GetTextureManager().GetTexture(L"tgelogo_d.dds");
		spriteSharedData.myTexture = texture;

		// Set normal map
		spriteSharedData.myMaps[Tga::NORMAL_MAP] = engine.GetTextureManager().GetTexture(L"tgelogo_n.dds");

		// Create instance data. 
		Tga::Sprite2DInstanceData spriteInstance = {};

		// Setting the pivot so all operations will be in the middle of the image (rotation, position, etc.)
		spriteInstance.myPivot = Tga::Vector2f(0.5f, 0.5f);
		spriteInstance.mySize = texture->mySize;

		const int numberOfLogos = 50;
		std::vector<Tga::Vector2f> positions;
		for (int i = 0; i < numberOfLogos; i++)
		{
			float randX = (float)(rand() % 100) * 0.01f;
			float randY = (float)(rand() % 100) * 0.01f;
			positions.push_back({ randX , randY });
		}

		Tga::Engine::GetInstance()->SetAmbientLightValue(0.1f);
		// LIGHT (Up to 8)

		srand((unsigned int)time(0));

#define RANOM_0_TO_1 (rand() % 100 / 100.0f)
#define RANOM_1_OR_0 (float)(rand() % 2)
		std::vector<Tga::PointLight> myLights;
		for (int i = 0; i < NUMBER_OF_LIGHTS_ALLOWED; i++)
		{
			Tga::PointLight light(Tga::Transform(Tga::Vector3f(RANOM_0_TO_1, RANOM_0_TO_1, 0.0)), Tga::Color(RANOM_1_OR_0, RANOM_1_OR_0, RANOM_1_OR_0, 1), 0.1f + (RANOM_0_TO_1 + RANOM_0_TO_1), 0.2f);
			myLights.push_back(light);
		}

		float timer = 0;
		while (true)
		{

			// Setting a custom camera, so the X coords go between 0 and 1:
			Tga::Camera camera;
			camera.SetOrtographicProjection(0.f, 1.f, 0.f, 1.f * engine.GetWindowRatioInversed(), 0.f, 1.f);
			engine.GetGraphicsEngine().SetCamera(camera);

			timer += 1.0f / 60.0f;
			if (!Tga::Engine::GetInstance()->BeginFrame())
			{
				break;
			}

			Tga::LightManager& lightManager = Tga::Engine::GetInstance()->GetLightManager();
			lightManager.ClearPointLights();
			for (Tga::PointLight& light : myLights)
			{
				lightManager.AddPointLight(light);
			}

			{
				Tga::SpriteBatchScope batch = spriteDrawer.BeginBatch(spriteSharedData);

				// Render all the loggos onto the sprite
				for (int i = 0; i < numberOfLogos; i++)
				{
					spriteInstance.myPosition = { positions[i].x, positions[i].y };
					spriteInstance.myRotation = static_cast<float>(cos(timer * 0.5f * sin(positions[i].x)));
					batch.Draw(spriteInstance);
				}
			}

			Tga::Engine::GetInstance()->EndFrame();
		}
	}
	Tga::Engine::GetInstance()->Shutdown();
}
