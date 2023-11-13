#include <string>
#include <tge/engine.h>
#include <tge/error/ErrorManager.h>

#include <tge/settings/settings.h>

// Set USE_CONSOLE_COMMAND to 1 to enable the console window for logging and error
// messages etc.
//
#define USE_CONSOLE_COMMAND (1)


// If USE_CONSOLE_COMMAND is set then link the application as a windows console
// application.
#if USE_CONSOLE_COMMAND
#pragma comment(linker, "/SUBSYSTEM:console")
#else
#pragma comment(linker, "/SUBSYSTEM:windows")
#endif // USE_CONSOLE_COMMAND


// Making sure that DX2DEngine lib is linked
//
#ifdef _DEBUG
#pragma comment(lib,"Engine_Debug.lib")
#pragma comment(lib,"External_Debug.lib")
#else
#pragma comment(lib,"Engine_Release.lib")
#pragma comment(lib,"External_Release.lib")
#endif // _DEBUG


// This is where the application actually starts
//
void Go( void );


#if USE_CONSOLE_COMMAND
// This is the application starting point used if we start from a console window.
// The WinMain function should copy this except for how console parameters are
// handled.
//
int main( const int /*argc*/, const char * /*argc*/[] )
{
    Go();

    return 0;
}
#else
// This is the application starting point used if we start without a console window.
// The functionality inside is identical to that of the standard main function. If we
// wish to check console arguments it will have to be done a bit differently from
// the console version.
//
int WINAPI WinMain( HINSTANCE /*hInstance*/, HINSTANCE /*hPrevInstance*/, char*, int /*nShowCmd*/ )
{
    Go();

    return 0;
}
#endif // USE_CONSOLE_COMMAND


// This is where the application starts of for real. By keeping this in it's own file
// we will have the same behaviour for both console and windows startup of the
// application.
//
void Go( void )
{
    Tga::LoadSettings(TGE_PROJECT_SETTINGS_FILE);

    // Lets keep the window size as hardcoded constants for now.
    //
    const unsigned short windowWidth  = 1280;
    const unsigned short windowHeight = 720;

    // Here the settings used when setting up the DX2D-engine are set.
    Tga::EngineConfiguration createParameters;
    createParameters.myActivateDebugSystems = Tga::DebugFeature::Fps
                                            | Tga::DebugFeature::Mem
                                            | Tga::DebugFeature::Filewatcher
                                            | Tga::DebugFeature::Cpu
                                            | Tga::DebugFeature::Drawcalls
											| Tga::DebugFeature::FpsGraph;

    createParameters.myWindowSize = { static_cast<unsigned short>(windowWidth), static_cast<unsigned short>(windowHeight) };
    createParameters.myRenderSize = { static_cast<unsigned short>(windowWidth), static_cast<unsigned short>(windowHeight) };
    createParameters.myEnableVSync          = true;
	createParameters.myUseLetterboxAndPillarbox = true;

    std::wstring appname = L"TGE: Tutorial 1";
#ifdef _DEBUG
    appname += L" [Debug]";
#endif

    createParameters.myApplicationName = appname;

    if( !Tga::Engine::GetInstance()->Start(createParameters) )
    {
        ERROR_PRINT( "Fatal error! Engine could not start!" );
        system("pause");
        exit( 0 );
    }

	while (true)
	{
		if (!Tga::Engine::GetInstance()->BeginFrame())
		{
			break;
		}

		Tga::Engine::GetInstance()->EndFrame();
	}
	Tga::Engine::GetInstance()->Shutdown();
}
