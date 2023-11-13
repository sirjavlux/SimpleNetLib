#include <string>
#include <tge/engine.h>
#include <tge/error/ErrorManager.h>

#include "../../TutorialCommon/TutorialCommon.h"
#include <tge/input/XInput.h>
#include <tge/drawers/DebugDrawer.h>

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

using namespace std::placeholders;
using namespace Tga;

void Go( void );
int main( const int /*argc*/, const char * /*argc*/[] )
{
    Go();
    return 0;
}

// Run with  Left thumb stick, jump with A, shoot with X
void Go( void )
{
	Tga::LoadSettings(TGE_PROJECT_SETTINGS_FILE);
	{
		TutorialCommon::Init(L"TGE: Tutorial 13, Gamepad");

		XInput myInput;
		DebugDrawer& debugDrawer = Tga::Engine::GetInstance()->GetDebugDrawer();
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

			debugDrawer.DrawCircle(Tga::Vector2f{ 0.45f, 0.7f }*resolution, 0.01f * resolution.y, myInput.IsConnected(0) ? Color(0, 1, 0, 1) : Color(1, 0, 0, 1));
			debugDrawer.DrawCircle(Tga::Vector2f{ 0.45f, 0.7f }*resolution, 0.005f * resolution.y, myInput.IsConnected(0) ? Color(0, 1, 0, 1) : Color(1, 0, 0, 1));

			// left Thumb
			float leftX = (myInput.leftStickX) * 0.1f + 0.3f;
			float leftY = (myInput.leftStickY) * 0.1f + 0.5f;
			debugDrawer.DrawCircle(Tga::Vector2f{ leftX,leftY }*resolution, 0.01f * resolution.y);
			debugDrawer.DrawCircle(Tga::Vector2f{ 0.3f, 0.5f }*resolution, 0.1f * resolution.y);

			float rightX = (myInput.rightStickX) * 0.1f + 0.6f;
			float rightY = (myInput.rightStickY) * 0.1f + 0.5f;
			debugDrawer.DrawCircle(Tga::Vector2f{ rightX,rightY }*resolution, 0.01f * resolution.y);
			debugDrawer.DrawCircle(Tga::Vector2f{ 0.6f, 0.5f }*resolution, 0.1f * resolution.y);


			debugDrawer.DrawCircle(Tga::Vector2f{ 0.45f, 0.6f }*resolution, 0.01f * resolution.y, myInput.IsPressed(XINPUT_GAMEPAD_A) ? Color(0, 1, 0, 1) : Color(1, 0, 0, 1));
			debugDrawer.DrawCircle(Tga::Vector2f{ 0.45f, 0.56f }*resolution, 0.01f * resolution.y, myInput.IsPressed(XINPUT_GAMEPAD_B) ? Color(0, 1, 0, 1) : Color(1, 0, 0, 1));
			debugDrawer.DrawCircle(Tga::Vector2f{ 0.45f, 0.52f }*resolution, 0.01f * resolution.y, myInput.IsPressed(XINPUT_GAMEPAD_X) ? Color(0, 1, 0, 1) : Color(1, 0, 0, 1));
			debugDrawer.DrawCircle(Tga::Vector2f{ 0.45f, 0.48f }*resolution, 0.01f * resolution.y, myInput.IsPressed(XINPUT_GAMEPAD_Y) ? Color(0, 1, 0, 1) : Color(1, 0, 0, 1));

			debugDrawer.DrawCircle(Tga::Vector2f{ 0.45f, 0.42f }*resolution, 0.01f * resolution.y, Color(myInput.leftTrigger, 1, 0, 1));
			debugDrawer.DrawCircle(Tga::Vector2f{ 0.45f, 0.38f }*resolution, 0.01f * resolution.y, Color(myInput.rightTrigger, 1, 0, 1));

			debugDrawer.DrawCircle(Tga::Vector2f{ 0.42f, 0.34f }*resolution, 0.01f * resolution.y, myInput.IsPressed(XINPUT_GAMEPAD_LEFT_SHOULDER) ? Color(0, 1, 0, 1) : Color(1, 0, 0, 1));
			debugDrawer.DrawCircle(Tga::Vector2f{ 0.48f, 0.34f }*resolution, 0.01f * resolution.y, myInput.IsPressed(XINPUT_GAMEPAD_RIGHT_SHOULDER) ? Color(0, 1, 0, 1) : Color(1, 0, 0, 1));


			debugDrawer.DrawCircle(Tga::Vector2f{ 0.42f, 0.30f }*resolution, 0.01f * resolution.y, myInput.IsPressed(XINPUT_GAMEPAD_LEFT_THUMB) ? Color(0, 1, 0, 1) : Color(1, 0, 0, 1));
			debugDrawer.DrawCircle(Tga::Vector2f{ 0.48f, 0.30f }*resolution, 0.01f * resolution.y, myInput.IsPressed(XINPUT_GAMEPAD_RIGHT_THUMB) ? Color(0, 1, 0, 1) : Color(1, 0, 0, 1));

			debugDrawer.DrawCircle(Tga::Vector2f{ 0.45f, 0.22f }*resolution, 0.01f * resolution.y, myInput.IsPressed(XINPUT_GAMEPAD_DPAD_UP) ? Color(0, 1, 0, 1) : Color(1, 0, 0, 1));
			debugDrawer.DrawCircle(Tga::Vector2f{ 0.45f, 0.18f }*resolution, 0.01f * resolution.y, myInput.IsPressed(XINPUT_GAMEPAD_DPAD_DOWN) ? Color(0, 1, 0, 1) : Color(1, 0, 0, 1));
			debugDrawer.DrawCircle(Tga::Vector2f{ 0.42f, 0.20f }*resolution, 0.01f * resolution.y, myInput.IsPressed(XINPUT_GAMEPAD_DPAD_LEFT) ? Color(0, 1, 0, 1) : Color(1, 0, 0, 1));
			debugDrawer.DrawCircle(Tga::Vector2f{ 0.48f, 0.20f }*resolution, 0.01f * resolution.y, myInput.IsPressed(XINPUT_GAMEPAD_DPAD_RIGHT) ? Color(0, 1, 0, 1) : Color(1, 0, 0, 1));

			myInput.Refresh();

			Tga::Engine::GetInstance()->EndFrame();
		}
	}

	Tga::Engine::GetInstance()->Shutdown();
}
