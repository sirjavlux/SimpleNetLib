#include <string>

#include "../../TutorialCommon/TutorialCommon.h"
#include <tge/sprite/sprite.h>
#include <tge/engine.h>
#include <tge/light/LightManager.h>
#include <tge/primitives/CustomShape.h>
#include <tge/drawers/CustomShapeDrawer.h>
#include <tge/engine.h>
#include <tge/graphics/Camera.h>
#include <tge/graphics/GraphicsEngine.h>
#include <tge/settings/settings.h>

#include "../SnakeShape.h"
#include "../CMayaObject.h"

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
		TutorialCommon::Init(L"TGE: Tutorial 6", true);

		Tga::CustomShape2D myShape;
		CSnakeShape mySnake;
		mySnake.Init(0.6f);

		CMayaObject myMayaObject;
		myMayaObject.Init("from_maya.obj");

		float timer = 0;
		while (true)
		{
			timer += 1.0f / 60.0f;
			
			{
				if (!Tga::Engine::GetInstance()->BeginFrame())
				{
					break;
				}
			}

			{
				// Setting a custom animated camera:

				Tga::Camera camera;
				float scale = exp2(1.f + cosf(timer/3.f));
				float ratio = Tga::Engine::GetInstance()->GetWindowRatioInversed();
				camera.SetOrtographicProjection(-scale, scale, -scale * ratio, scale * ratio, 0.f, 1.f);
				Tga::Engine::GetInstance()->GetGraphicsEngine().SetCamera(camera);
			}

			{
				{
					myShape.Reset();
					myShape.AddPoint({ 0.3f, 0.5f }, Tga::Color(1, 0, 0, 1));
					myShape.AddPoint({ 0.7f, 0.5f }, Tga::Color(0, 1, 0, 1));
					myShape.AddPoint({ (cosf(timer) * 0.1f) + 0.5f,  (sinf(timer) * 0.3f) + 0.5f }, Tga::Color(0, 0, 1, 1));
					myShape.BuildShape();

					Tga::Engine::GetInstance()->GetGraphicsEngine().GetCustomShapeDrawer().Draw(myShape);
				}

				{
					mySnake.Update(1.0f / 60.0f);
				}

				{
					myMayaObject.Render(1.0f / 60.0f);
				}
			}

			{
				Tga::Engine::GetInstance()->EndFrame();
			}
		}
	}

	Tga::Engine::GetInstance()->Shutdown();
}
