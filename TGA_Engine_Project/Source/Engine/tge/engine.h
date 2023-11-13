/*
Engine
This class is the interface between game logic and rendering. 
It takes care of creating everything necessary for window handling and rendering
*/

#pragma once

#define WIN32_LEAN_AND_MEAN 
#define NOMINMAX 
#include <windows.h>

#include <functional>
#include <tge/math/color.h>
#include <tge/math/vector2.h>
#include <tge/render/RenderCommon.h>
#include <chrono>
#include "StepTimer.h"
#include <tge/EngineDefines.h>

namespace Tga
{
    class LightManager;
    class FileWatcher;
    class DX11;

#ifndef _RETAIL
    class ImGuiInterface;
#endif // !_RETAIL
}

#define TGA_DEFAULT_CRYSTAL_BLUE { 3.0f / 255.0f, 153.0f / 255.0f, 176.0f / 255.0f, 1.0f }

namespace Tga
{
	using callback_function_wndProc = std::function<LRESULT(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)>;
    using callback_function_log    = std::function<void( std::string )>;
    using callback_function_error  = std::function<void( std::string )>;

    enum class DebugFeature
    {
        None = 0,
        Fps = 1 << 0,
        Mem = 1 << 1,
        Drawcalls = 1 << 2,
        Cpu = 1 << 3,
        FpsGraph = 1 << 4,
        Filewatcher = 1 << 5,
        OptimizeWarnings = 1 << 6,
        MemoryTrackingStackTraces = 1 << 7,
        MemoryTrackingAllAllocations =  1 << 8,
        All = (1 << 9) - 1,
    };
    inline DebugFeature operator|(DebugFeature lhs, DebugFeature rhs)
    {
        return static_cast<DebugFeature>(static_cast<std::underlying_type<DebugFeature>::type>(lhs) | static_cast<std::underlying_type<DebugFeature>::type>(rhs));
    }
    inline DebugFeature operator&(DebugFeature lhs, DebugFeature rhs)
    {
        return static_cast<DebugFeature>(static_cast<std::underlying_type<DebugFeature>::type>(lhs) & static_cast<std::underlying_type<DebugFeature>::type>(rhs));
    }
    enum class WindowSetting
    {
        Overlapped,
        Borderless,
    };
	enum class MultiSamplingQuality
	{
		Off,	
		Low = 1,
		Medium = 2,
		High = 3,
	};

    struct EngineConfiguration
    {
        EngineConfiguration()
        { 
            myHwnd							= nullptr; 
            myHInstance						= nullptr;  
            myWindowSize                    = { 1280, 720 };
            myEnableVSync					= false; 
            myRenderSize                    = myWindowSize;
            myErrorFunction					= nullptr; 
			myLogFunction					= nullptr;
			myWinProcCallback				= nullptr;
            myStartInFullScreen				= false;
            myWindowSetting					= WindowSetting::Overlapped;
            myTargetSize                    = myWindowSize;
            myUseLetterboxAndPillarbox		= false;
            myActivateDebugSystems			= DebugFeature::Fps | DebugFeature::Mem;
			myPreferedMultiSamplingQuality	= MultiSamplingQuality::Off;
			myClearColor					= TGA_DEFAULT_CRYSTAL_BLUE;
        }

		callback_function_wndProc myWinProcCallback;
        callback_function_log myLogFunction;
        callback_function_error myErrorFunction;

		/* How big should the window be? */
        Vector2ui myWindowSize;

		/* What resolution should we render everything in?*/
        Vector2ui myRenderSize;

		/* In what resolution are the graphic artist working?*/
        Vector2ui myTargetSize;

        /* Will show the FPS and memory text*/
        DebugFeature myActivateDebugSystems;
        Color myClearColor;
        HWND *myHwnd;
        HINSTANCE myHInstance;
        std::wstring myApplicationName;
        bool myEnableVSync;
        bool myStartInFullScreen;
        WindowSetting myWindowSetting;
        bool myUseLetterboxAndPillarbox;
        
		MultiSamplingQuality myPreferedMultiSamplingQuality;
    };


    class Engine
    {
        friend class TextureManager;
        friend class Renderer;
        friend class Shader;
        friend class ErrorManager;
        friend class GraphicsEngine;
		friend class WindowsWindow;
		friend class ErrorManager;
		friend class CustomShapeDrawer;
		friend class LineDrawer;
		friend class DebugDrawer;
		friend class CustomShape;
		friend class Texture;
		friend class Text;
		friend class TextService;
		friend class Video;
		friend class Light;
		friend class ModelShape;

    public:
        Engine &operator =( const Engine &anOther ) = delete;
        static bool Start(const EngineConfiguration& aWindowConfiguration = EngineConfiguration());
        static bool StartServer(const EngineConfiguration& aWindowConfiguration = EngineConfiguration());
		static void Shutdown();

        static Engine* GetInstance() {return myInstance;}

        TextureManager& GetTextureManager() const {return *myTextureManager;}
        DebugDrawer& GetDebugDrawer() const {return *myDebugDrawer;}
		ErrorManager& GetErrorManager() const { return *myErrorManager; }

		Vector2ui GetWindowSize() const { return myWindowConfiguration.myWindowSize; }
		Vector2ui GetRenderSize() const { return myWindowConfiguration.myRenderSize; }
		Vector2ui GetTargetSize() const { return myWindowConfiguration.myTargetSize; }

        float GetWindowRatio() const;
        float GetWindowRatioInversed() const;

		float GetDeltaTime() const { return myDeltaTime; }
        float GetTotalTime() const { return myTotalTime; }
        HWND* GetHWND() const;
        HINSTANCE GetHInstance() const;
      
		bool IsDebugFeatureOn(DebugFeature aFeature) const;

		void SetTargetSize(const Vector2ui& aResolution);
		void SetResolution(const Vector2ui &aResolution, bool aAlsoSetWindowSize = true);
        void SetFullScreen(bool aFullScreen);
		
		void SetClearColor(const Color& aClearColor);
        FORCEINLINE const Color& GetClearColor() const { return myWindowConfiguration.myClearColor; }
		// If you want to manually tell the engine to render instead of the callback function with the (myUpdateFunctionToCall)
		bool BeginFrame();
		void EndFrame(void);

        LightManager& GetLightManager() const { return *myLightManager; }

		void SetAmbientLightValue(float aAmbientLight);
		GraphicsEngine& GetGraphicsEngine() const { return *myGraphicsEngine; }

    private:    // Private interface
        Engine(const EngineConfiguration& aWindowConfiguration);
        ~Engine();

        static void DestroyInstance();

		FileWatcher* GetFileWatcher() { return myFileWatcher.get(); }
		TextService& GetTextService() const { return *myTextService; }
		WindowsWindow& GetWindow() const { return *myWindow; }
		
		bool InternalStart();

        void CalculateRatios();
		void SetViewPort(float aTopLeftX, float aTopLeftY, float aWidth, float aHeight, float aMinDepth = 0.0f, float aMaxDepth = 1.0f, bool aSetEngineResolution = false);
		void SetWantToUpdateSize() { myWantToUpdateSize = true; }

		void UpdateWindowSizeChanges(bool aIgnoreAutoUpdate = false);

		Vector2f GetWindowRatioVec() const;
		Vector2f GetWindowRatioInversedVec() const;

    private:    // Private data
        static Engine* myInstance;

        std::unique_ptr<WindowsWindow> myWindow;
        std::unique_ptr<DX11> myDx11;
        std::unique_ptr<GraphicsEngine> myGraphicsEngine;
        std::unique_ptr<TextureManager> myTextureManager;
        std::unique_ptr<DebugDrawer> myDebugDrawer;
        std::unique_ptr<TextService> myTextService;

        EngineConfiguration myWindowConfiguration;

        std::unique_ptr<LightManager> myLightManager;
        std::unique_ptr<ErrorManager> myErrorManager;
        std::unique_ptr<FileWatcher> myFileWatcher;
		bool myWantToUpdateSize;

        bool myRunEngine;
        float myWindowRatio;
        float myWindowRatioInversed;
		Vector2f myWindowRatioVec;
		Vector2f myWindowRatioInversedVec;

		std::chrono::steady_clock::time_point myStartOfTime;
        float myTotalTime;
        float myDeltaTime;

		DX::StepTimer myTimer;

		bool myShouldExit; // Only used when using beginframe and endframe

#ifndef _RETAIL
        std::unique_ptr<ImGuiInterface> myImguiInterFace = nullptr;
#endif // !_RETAIL
    };
}
