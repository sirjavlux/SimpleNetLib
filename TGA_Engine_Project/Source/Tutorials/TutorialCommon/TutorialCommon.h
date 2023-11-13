#pragma once
#include <tge/engine.h>
#include <tge/error/ErrorManager.h>

namespace TutorialCommon
{
	void Init(std::wstring aAppName, bool aFullDebug = false, bool aVsyncEnable = true)
	{
		unsigned short windowWidth = 1920;
		unsigned short windowHeight = 1080;

		Tga::EngineConfiguration createParameters;
		if (aFullDebug)
		{
			createParameters.myActivateDebugSystems = Tga::DebugFeature::Fps | Tga::DebugFeature::Mem | Tga::DebugFeature::Filewatcher | Tga::DebugFeature::Cpu | Tga::DebugFeature::Drawcalls | Tga::DebugFeature::OptimizeWarnings;

		}
		else
		{
			createParameters.myActivateDebugSystems = Tga::DebugFeature::Filewatcher;
		}

		createParameters.myWindowSize = { windowWidth, windowHeight };
		createParameters.myRenderSize = { windowWidth, windowHeight };
		createParameters.myTargetSize = { windowWidth, windowHeight };
		createParameters.myWindowSetting = Tga::WindowSetting::Overlapped;
		//createParameters.myAutoUpdateViewportWithWindow = true;
		createParameters.myStartInFullScreen = false;
		createParameters.myPreferedMultiSamplingQuality = Tga::MultiSamplingQuality::High;

		createParameters.myApplicationName = aAppName;
		createParameters.myEnableVSync = aVsyncEnable;

		if (!Tga::Engine::GetInstance()->Start(createParameters))
		{
			ERROR_PRINT("Fatal error! Engine could not start!");
			system("pause");
	
		}
	}
}