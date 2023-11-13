#include <string>

#include <tge/drawers/DebugDrawer.h>
#include <tge/math/vector2.h>
#include <tge/engine.h>
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
		TutorialCommon::Init(L"TGE: Tutorial 8", true);

		Tga::Vector2f targetPos;

		auto winsize = Tga::Engine::GetInstance()->GetTargetSize();

		const float deltaTime = 1.0f / 60.0f;
		float timer = 0.0f;
		while (true)
		{
			timer += deltaTime;
			if (!Tga::Engine::GetInstance()->BeginFrame())
			{
				break;
			}

			targetPos.Set(0.5f*winsize.x, (cosf(timer) * 0.2f) + 0.5f*winsize.y);

			// Arrow
			{
				Tga::Vector2f from(0, 0.5f*winsize.y);
				Tga::Vector2f to(0.5f*winsize.x, 0.5f*winsize.y);
				Tga::Color color(1, 0, 0, 1);
				Tga::Engine::GetInstance()->GetDebugDrawer().DrawArrow(from, targetPos, color);
			}

			//Circle
			{
				Tga::Engine::GetInstance()->GetDebugDrawer().DrawCircle({ 0.3f*winsize.x, 0.4f*winsize.y}, 30.0f + (cosf(timer) * 0.1f), Tga::Color(cosf(timer), -cosf(timer * 3), sinf(timer), 1));
			}

			//Lines
			struct Line
			{
				Tga::Vector2f myFrom;
				Tga::Vector2f myTo;
			};

			// Single Line
			{
				Line line;
				line.myFrom.Set(0.5f*winsize.x, 0.1f*winsize.y);
				line.myTo.Set(0.6f*winsize.x, 0.3f*winsize.y);
				Tga::Engine::GetInstance()->GetDebugDrawer().DrawLine(line.myFrom, line.myTo, { 1, 1, 1, 1 });
			}

			Tga::Engine::GetInstance()->EndFrame();
		}
		Tga::Engine::GetInstance()->Shutdown();
	}
}
