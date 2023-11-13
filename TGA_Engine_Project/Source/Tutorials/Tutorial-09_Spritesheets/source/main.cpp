#include <string>

#include <tge/drawers/DebugDrawer.h>
#include <tge/math/vector2.h>
#include <tge/engine.h>
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


void Go( void )
{
	Tga::LoadSettings(TGE_PROJECT_SETTINGS_FILE);
	{
		TutorialCommon::Init(L"TGE: Tutorial 9");

		Tga::Engine& engine = *Tga::Engine::GetInstance();

		// Get a sprite drawer, for drawing sprites
		Tga::SpriteDrawer& spriteDrawer(engine.GetGraphicsEngine().GetSpriteDrawer());

		// Sprite data is split into two parts. Shared data that is shared across multiple sprites and instance data that is unique per instance.
		// This split is done for performance reasons.

		// Create shared data and assign a texture.
		Tga::SpriteSharedData sharedData = {};
		Tga::Texture* texture = engine.GetTextureManager().GetTexture(L"animation.dds");
		sharedData.myTexture = texture;

		Tga::Vector2ui intResolution = Tga::Engine::GetInstance()->GetRenderSize();
		Tga::Vector2f resolution = { (float)intResolution.x, (float)intResolution.y };


		struct UV
		{
			UV(Tga::Vector2f aStart, Tga::Vector2f aEnd) { myStart = aStart; myEnd = aEnd; }
			Tga::Vector2f myStart;
			Tga::Vector2f myEnd;
		};

		const float addingUVX = 1.0f / 8.0f; // 8 sprites per row
		const float addingUVY = 1.0f / 8.0f; // 8 sprites per col
		std::vector<UV> myUvs;
		for (int j = 0; j < 8; j++)
		{
			for (int i = 0; i < 8; i++)
			{
				myUvs.push_back(UV({ addingUVX * i, addingUVY * j }, { (addingUVX * i) + addingUVX, (addingUVY * j) + addingUVY }));
			}
		}

		const float deltaTime = 1.0f / 60.0f;
		float timer = 0.0f;
		unsigned short aIndex = 0;
		while (true)
		{
			timer += deltaTime;
			if (!Tga::Engine::GetInstance()->BeginFrame())
			{
				break;
			}

			// Cycle the sheet
			if (timer >= 0.05f)
			{
				aIndex++;
				if (aIndex > 28)
				{
					aIndex = 0;
				}
				timer = 0.0f;
			}

			Tga::Sprite2DInstanceData instance = {};
			instance.mySize = texture->myImageSize / 8.f; // Setting size to an eigth of scale to get pixel size to match screen
			instance.myPosition = resolution * 0.5f;
			instance.myPivot = { 0.5f, 0.5f };
			instance.myTextureRect = { myUvs[aIndex].myStart.x, myUvs[aIndex].myStart.y, myUvs[aIndex].myEnd.x, myUvs[aIndex].myEnd.y };
			spriteDrawer.Draw(sharedData, instance);

			Tga::Engine::GetInstance()->EndFrame();
		}
	}

	Tga::Engine::GetInstance()->Shutdown();
}
