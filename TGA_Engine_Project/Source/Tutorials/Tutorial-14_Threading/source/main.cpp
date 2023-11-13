#include <string>
#include <tge/engine.h>
#include <tge/graphics/GraphicsEngine.h>
#include <tge/sprite/sprite.h>
#include <tge/drawers/SpriteDrawer.h>
#include <tge/texture/TextureManager.h>
#include <tge/error/ErrorManager.h>

#include "../../TutorialCommon/TutorialCommon.h"
#include <thread>
#include <vector>
#include <mutex>
#include <tge/text/text.h>
#include <chrono>

#include <tge/settings/settings.h>

#define SIMULATE_HEAVY_RESOURCE_LOADING false
#define SIMULATE_GAMEPLAY_PERFORMANCE_ISSUES false
#define SIMULATE_ENGINE_PERFORMANCE_ISSUES false

using namespace std::chrono_literals;

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


bool myShouldExit = false;
bool myIsEngineStarted = false;
std::thread *myRenderThread = nullptr;
std::thread *myLoadingThread = nullptr;
std::thread *myGameThreadThread = nullptr;

unsigned int myIDDistribution = 0;

// This is a struct that only lives on the engine/loading side. Gameplay should never ever touch the sprite data
struct SpriteResourceEngine
{
	unsigned int myID = 0;
	Tga::SpriteSharedData mySharedData = {};
	Tga::Sprite2DInstanceData myInstanceData = {};
};

//This lives on the gameplay side, the only thing connecting this to a sprite is the ID
struct SpriteResourceGame
{
	unsigned int myID = 0;
	Vector2f myPosition;
};

std::mutex myLoadingEngineTransferMutex; // When resources are loaded, we need to send them to the engine
std::mutex myGameEngineTransferMutex; // When we want to tell the engine what to draw
std::vector<SpriteResourceEngine> myLoadedResources; // These resources live only in the loading thread and engine thread
std::vector<SpriteResourceGame> myGameSpritesToBeRendered; // These resources live in engine and in game
int myLoadingThreaFPS = 0; // Just to show fps
int myGameThreadFPS = 0;// Just to show fps

void RunEngineThread();
void RunLoadingThread();
void RunGameThread();
int main( const int /*argc*/, const char * /*argc*/[] )
{
	Tga::LoadSettings(TGE_PROJECT_SETTINGS_FILE);

	myRenderThread = new std::thread(RunEngineThread); // Create render thread
	myLoadingThread = new std::thread(RunLoadingThread); // Create loading thread
	myGameThreadThread = new std::thread(RunGameThread); // Create game thread
   
	// the main thread is now unused, this is a waste in a real game, we should use the main as some kind of worker
	while (!myShouldExit)
	{
		// Main thread should just wait
		std::this_thread::sleep_for(std::chrono::milliseconds(16));
	}

	// Sync and shut down all threads
	if (myRenderThread)
	{
		myRenderThread->join();
	}
	if (myLoadingThread)
	{
		myLoadingThread->join();
	}
	if (myGameThreadThread)
	{
		myGameThreadThread->join();
	}

    return 0;
}


void RunEngineThread()
{
	{
		TutorialCommon::Init(L"TGE: Tutorial 14, threading", true, false);
		myIsEngineStarted = true;
		std::vector<SpriteResourceEngine> myEngineSprites;

		Tga::Vector2ui intResolution = Tga::Engine::GetInstance()->GetRenderSize();
		Tga::Vector2f resolution = { (float)intResolution.x, (float)intResolution.y };

		Tga::Text textEngineFPS(L"Text/BarlowSemiCondensed-Regular.ttf", Tga::FontSize_18);
		textEngineFPS.SetText("");
		textEngineFPS.SetPosition(Tga::Vector2f{ 0.1f, 0.9f } * resolution);
		textEngineFPS.SetColor(Tga::Color(1.0f, 1.0f, 1.0f, 1.0f));

		Tga::Text textLoaderFPS(L"Text/BarlowSemiCondensed-Regular.ttf", Tga::FontSize_18);
		textLoaderFPS.SetText("");
		textLoaderFPS.SetPosition(Tga::Vector2f{ 0.1f, 0.88f } * resolution);
		textLoaderFPS.SetColor(Tga::Color(1.0f, 1.0f, 1.0f, 1.0f));

		Tga::Text textGameFPS(L"Text/BarlowSemiCondensed-Regular.ttf", Tga::FontSize_18);
		textGameFPS.SetText("");
		textGameFPS.SetPosition(Tga::Vector2f{ 0.1f, 0.86f } * resolution);
		textGameFPS.SetColor(Tga::Color(1.0f, 1.0f, 1.0f, 1.0f));


		using clock = std::chrono::high_resolution_clock;
		auto time_start = clock::now();

		Tga::Engine& engine = *Tga::Engine::GetInstance();
		Tga::SpriteDrawer& spriteDrawer(engine.GetGraphicsEngine().GetSpriteDrawer());

		while (true)
		{
			std::chrono::duration<double, std::milli> delta_time = clock::now() - time_start;
			time_start = clock::now();


			// If there are new resources loaded, transfer ownership to the render
			while (myLoadedResources.size() > 0)
			{
				myLoadingEngineTransferMutex.lock();
				myEngineSprites.push_back(myLoadedResources[0]);
				myLoadedResources.erase(myLoadedResources.begin());
				myLoadingEngineTransferMutex.unlock();
			}


			//Render all game sprites that is pushed from the game thread
			std::vector<SpriteResourceGame> toRenderSprites;
			myGameEngineTransferMutex.lock();
			toRenderSprites = (myGameSpritesToBeRendered);
			myGameEngineTransferMutex.unlock();

			if (!engine.BeginFrame())
			{
				myShouldExit = true;
				break;
			}

			for (SpriteResourceGame& gameSprite : toRenderSprites)
			{
				if (myEngineSprites.size() > gameSprite.myID)
				{
					myEngineSprites[gameSprite.myID].myInstanceData.myPosition = gameSprite.myPosition;
					spriteDrawer.Draw(myEngineSprites[gameSprite.myID].mySharedData, myEngineSprites[gameSprite.myID].myInstanceData);
				}

			}
			textEngineFPS.SetText("Engine thread FPS: " + std::to_string((int)(1.0f / (delta_time.count() / 1000.0f))));
			textEngineFPS.Render();

			textLoaderFPS.SetText("Loading thread FPS: " + std::to_string(myLoadingThreaFPS));
			textLoaderFPS.Render();

			textGameFPS.SetText("Game thread FPS: " + std::to_string(myGameThreadFPS));
			textGameFPS.Render();

			engine.EndFrame();

			if (SIMULATE_ENGINE_PERFORMANCE_ISSUES)
			{
				std::this_thread::sleep_for(std::chrono::milliseconds(70));
			}
			else
			{
				std::this_thread::yield();
			}
		}
	}

	Tga::Engine::GetInstance()->Shutdown();
}

std::vector<std::wstring> myToLoadResources;
void RunLoadingThread()
{
	while (!myIsEngineStarted) // Wait for engine to start
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(16));
	}

	// Here we should get all resources from a file/level
	const unsigned int numberOfResources = 100;
	for (int i=0; i< numberOfResources; i++)
	{
		myToLoadResources.push_back(L"sprites/tge_logo_b.dds");
	}

	using clock = std::chrono::high_resolution_clock;
	auto time_start = clock::now();

	while (!myShouldExit)
	{
		std::chrono::duration<double, std::milli> delta_time = clock::now() - time_start;
		time_start = clock::now();


		Tga::Vector2ui intResolution = Tga::Engine::GetInstance()->GetRenderSize();
		Tga::Vector2f resolution = { (float)intResolution.x, (float)intResolution.y };

		if (myToLoadResources.size() > 0)
		{
			SpriteResourceEngine newEngineResource;
			newEngineResource.myID = myIDDistribution;
			newEngineResource.mySharedData.myTexture = Tga::Engine::GetInstance()->GetTextureManager().GetTexture(myToLoadResources[0].c_str());
			newEngineResource.myInstanceData.mySize = resolution.x*0.05f;
			newEngineResource.myInstanceData.myPivot = { 0.5f, 0.5f };
			
			myToLoadResources.erase(myToLoadResources.begin());
			myIDDistribution++;

			myLoadingEngineTransferMutex.lock();
			myLoadedResources.push_back(newEngineResource);
			myLoadingEngineTransferMutex.unlock();
		}

		myLoadingThreaFPS = (int)(1.0f / (delta_time.count() / 1000.0f));
		if (SIMULATE_HEAVY_RESOURCE_LOADING)
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(40));
		}
		else
		{
			std::this_thread::yield();
		}
	}
}

void RunGameThread()
{
	while (!myIsEngineStarted) // Wait for engine to start
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(1));
	}

	using clock = std::chrono::high_resolution_clock;
	auto time_start = clock::now();

	std::vector<SpriteResourceGame> spritesThatShouldBeSendToEngineForRender;
	spritesThatShouldBeSendToEngineForRender.reserve(100);

	Tga::Vector2ui intResolution = Tga::Engine::GetInstance()->GetRenderSize();
	Tga::Vector2f resolution = { (float)intResolution.x, (float)intResolution.y };

	float xOffset = 0.05f;

	for (int i = 0; i < 100; i++)
	{
		SpriteResourceGame spriteToRender;
		spriteToRender.myID = i % 100;
		spriteToRender.myPosition.x = (1.f + 2.f * xOffset) * resolution.x * static_cast<float>(i % 10) / 10.0f - xOffset;
		spriteToRender.myPosition.y = resolution.y * static_cast<float>(i / 10) / 10.0f;
		spritesThatShouldBeSendToEngineForRender.push_back(spriteToRender);
	}

	float myTotalTime = 0;
	while (!myShouldExit) // Wait for engine to start
	{

		std::chrono::duration<double, std::milli> delta_time = clock::now() - time_start;
		time_start = clock::now();

		float fDeltaTime = (static_cast<float>(delta_time.count()) / 1000.0f);
		myTotalTime += fDeltaTime;
		for (SpriteResourceGame& gameResource : spritesThatShouldBeSendToEngineForRender)
		{
			gameResource.myPosition.x += exp2(sinf(5.f*(gameResource.myPosition.x + gameResource.myPosition.y) / resolution.x + myTotalTime))*0.1f*resolution.x * fDeltaTime;

			if (gameResource.myPosition.x > resolution.x * (1.f + xOffset))
				gameResource.myPosition.x -= resolution.x * (1.f + 2.f* xOffset);
		}

		myGameEngineTransferMutex.lock();
	
		myGameSpritesToBeRendered = (spritesThatShouldBeSendToEngineForRender);
		myGameEngineTransferMutex.unlock();

		myGameThreadFPS = (int)(1.0f / (delta_time.count() / 1000.0f));

		if (SIMULATE_GAMEPLAY_PERFORMANCE_ISSUES)
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(100));
		}
		else
		{
			std::this_thread::yield();
		}
	}
}