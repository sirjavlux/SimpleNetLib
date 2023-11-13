#include <string>
#include <tge/engine.h>
#include <tge/error/ErrorManager.h>

#include <tge/input/XInput.h>
#include "../../TutorialCommon/TutorialCommon.h"
#include <tge/audio/audio.h>
#include <tge/animation/AnimationPlayer.h>
#include <tge/graphics/Camera.h>
#include <tge/graphics/dx11.h>
#include <tge/graphics/GraphicsEngine.h>
#include <tge/drawers/ModelDrawer.h>
#include <tge/texture/TextureManager.h>
#include <tge/model/modelfactory.h>

#include <tge/Timer.h>
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

Tga::Audio myFootstepAudios[4];
Tga::Audio myShootAudio;

/* This function will trigger as soon as an animation is sending an event from the animation, could be anything! Footsteps, something spawning, anything really
The animators will add these events in spine, and set what should be send, maybe a string, maybe a float or integer, thats why there are so many in the function.
*/
void AnimCallback()
{
	
}

// Run with  Left thumb stick, jump with A, shoot with X
void Go(void)
{
	Tga::LoadSettings(TGE_PROJECT_SETTINGS_FILE);
	{
		TutorialCommon::Init(L"TGE: Tutorial 11");

		// TODO: DB 2022-04-07 Disabled events for the time being due to change in import stack being rushed. Will be reimplemented as an upgrade.
		//myFootstepAudios[0].Init("../source/tutorials/Tutorial11SkinnedAnimation/data/audio/footstep_wood_run_01.wav");
		//myFootstepAudios[1].Init("../source/tutorials/Tutorial11SkinnedAnimation/data/audio/footstep_wood_run_02.wav");
		//myFootstepAudios[2].Init("../source/tutorials/Tutorial11SkinnedAnimation/data/audio/footstep_wood_run_03.wav");
		//myFootstepAudios[3].Init("../source/tutorials/Tutorial11SkinnedAnimation/data/audio/footstep_wood_run_04.wav");

		myShootAudio.Init("audio/sci-fi_weapon_blaster_laser_boom_small_02.wav");

		// Create the sprite with the path to the image
		ModelFactory& modelFactory = ModelFactory::GetInstance();
		Tga::AnimatedModelInstance model = modelFactory.GetAnimatedModelInstance(L"ani/popp_sk.fbx");
		model.SetLocation({ 0.0f, -0.25f, 0.5f });
		model.SetScale(Vector3f(0.1f, 0.1f, 0.1f));
		XInput myInput;

		std::vector<AnimationPlayer> animations = 
		{ 
			modelFactory.GetAnimationPlayer(L"ani/sneak.fbx", model.GetModel()),
			modelFactory.GetAnimationPlayer(L"ani/idle.fbx", model.GetModel()),
			modelFactory.GetAnimationPlayer(L"ani/run.fbx", model.GetModel()),
			modelFactory.GetAnimationPlayer(L"ani/jump.fbx", model.GetModel())
		};
		animations[0].SetIsLooping(true);
		animations[1].SetIsLooping(true);
		animations[2].SetIsLooping(true);

		// TODO: DB 2022-04-07 Disabled events for the time being due to change in import stack being rushed. Will be reimplemented as an upgrade.
		//model.RegisterAnimationEventCallback("event_step", [] { myFootstepAudios[rand() % 3].Play(); });

		TextureResource* texture = Tga::Engine::GetInstance()->GetTextureManager().GetTexture(L"ani/atlas.tga");
		model.SetTexture(0, 0, texture);

		const int IDLE_INDEX = 1;
		const int RUNNING_INDEX = 2;
		const int JUMPING_INDEX = 3;
		Timer aTimer;

		int activeAnimationIndex = IDLE_INDEX;
		animations[IDLE_INDEX].Play();

		while (true)
		{
			if (!Tga::Engine::GetInstance()->BeginFrame())
			{
				break;
			}

			Vector2ui resolution = DX11::GetResolution();
			Tga::Camera camera;
			camera.SetOrtographicProjection(1.0, resolution.y / (float)resolution.x, 1.0f);
			Tga::Engine::GetInstance()->GetGraphicsEngine().SetCamera(camera);

			myInput.Refresh();

			if (activeAnimationIndex != JUMPING_INDEX)
			{
				if (myInput.leftStickX > 0.2f)
				{
					activeAnimationIndex = RUNNING_INDEX;
					animations[activeAnimationIndex].Play();
					model.SetScale(Vector3f(-0.1f, 0.1f, -0.1f));
					
				}
				else if (myInput.leftStickX < -0.2f)
				{
					activeAnimationIndex = RUNNING_INDEX;
					animations[activeAnimationIndex].Play();
					model.SetScale(Vector3f(0.1f,0.1f,-0.1f));
				}
				else
				{
					activeAnimationIndex = IDLE_INDEX;
					animations[activeAnimationIndex].Play();
				}
			}
			else if (activeAnimationIndex == JUMPING_INDEX)
			{
				if (animations[activeAnimationIndex].GetState() == AnimationState::Finished)
				{
					activeAnimationIndex = IDLE_INDEX;
					animations[activeAnimationIndex].Play();
				}
			}

			if (myInput.IsPressed(XINPUT_GAMEPAD_A))
			{
				if (activeAnimationIndex != JUMPING_INDEX)
				{
					activeAnimationIndex = JUMPING_INDEX;
					animations[activeAnimationIndex].Play();
				}

			}

			aTimer.Update();
			animations[activeAnimationIndex].Update(aTimer.GetDeltaTime());
			model.SetPose(animations[activeAnimationIndex]);

			Tga::Engine::GetInstance()->GetGraphicsEngine().GetModelDrawer().Draw(model);

			Tga::Engine::GetInstance()->EndFrame();
		}
	}

	Tga::Engine::GetInstance()->Shutdown();

}
