#include <string>
#include <tge/engine.h>
#include <tge/error/ErrorManager.h>
#include <tge/settings/settings.h>

#include "../../TutorialCommon/TutorialCommon.h"
#include <tge/audio/audio.h>


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
		TutorialCommon::Init(L"TGE: Tutorial 12, Audio");

		Tga::Audio audio;
		audio.Init("music.ogg", true);
		audio.Play();
		float timer = 0;
		while (true)
		{
			timer += 1.0f / 60.0f;
			if (!Tga::Engine::GetInstance()->BeginFrame())
			{
				break;
			}

			POINT p;
			GetCursorPos(&p);
			ScreenToClient(*Tga::Engine::GetInstance()->GetHWND(), &p);

			float panX = static_cast<float>(p.x) / static_cast<float>(Tga::Engine::GetInstance()->GetWindowSize().x);
			// Go from 0-1 space to -1-1
			panX *= 2.0f;
			panX -= 1.0f;

			float panY = 0; // In 2D, y is nothing if we use stereo

			audio.SetPosition({ panX , panY });

			Tga::Engine::GetInstance()->EndFrame();
		}
	}

	Tga::Engine::GetInstance()->Shutdown();

}
