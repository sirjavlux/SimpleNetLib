#pragma once
#include <vector>
#include <string>
#include <memory>
#include <tge/animation/animation.h>
#include <tge/animation/pose.h>
#include <tge/model/model.h>

namespace Tga
{
	enum class AnimationState : uint8_t
	{
		NoAnimation,
		NotStarted,
		Playing,
		Stopped,
		Paused,
		Finished,
	};

	/// <summary>
	/// Plays an Animation on a Model. Holds the active state of the animation and the resulting pose.
	/// </summary>
	class AnimationPlayer
	{
	private:
		std::shared_ptr<const Animation> myAnimation = nullptr;
		std::shared_ptr<const Model> myModel = nullptr;
		float myTime = 0;
		bool myIsLooping = false;
		bool myIsInterpolating = true;
		float myFPS = 0.f;

		AnimationState myState = AnimationState::NotStarted;

		LocalSpacePose myLocalSpacePose;

	public:
		void Init(const std::shared_ptr<const Animation>& animation, const std::shared_ptr<const Model>& model);

		/**
		 * Updates the animation and writes a new pose.
		 */
		void Update(float aDeltaTime);
		void UpdatePose();
		/**
		 * Instructs the animation to play.
		 */
		void AnimationPlayer::Play()
		{
			myState = AnimationState::Playing;
		}

		/**
		 * Pauses the animation at the current frame.
		 */
		void AnimationPlayer::Pause()
		{
			myState = AnimationState::Paused;
		}

		/**
		 * Stops and rewinds the current animation to the beginning.
		 */
		void AnimationPlayer::Stop()
		{
			myState = AnimationState::Stopped;
			myTime = 0;
		}

		bool GetIsLooping() const { return myIsLooping; }
		void AnimationPlayer::SetIsLooping(bool shouldLoop) { myIsLooping = shouldLoop; }

		bool GetIsInterpolating() const { return myIsInterpolating; }
		void AnimationPlayer::SetIsInterpolating(bool shouldInterpolate) { myIsInterpolating = shouldInterpolate; }

		float GetFramesPerSecond() const { return myFPS; }
		void AnimationPlayer::SetFramesPerSecond(float someFPS)
		{
			myFPS = someFPS;
		}

		float GetTime() const { return myTime; }
		void AnimationPlayer::SetTime(float time)
		{
			myTime = time;
		}

		void AnimationPlayer::SetFrame(unsigned int frame)
		{
			myTime = frame / myFPS;
		}

		const LocalSpacePose& GetLocalSpacePose() const { return myLocalSpacePose; }
		std::shared_ptr<const Animation> GetAnimation() const { return myAnimation; }
		AnimationState GetState() const { return myState; }
		
		unsigned int AnimationPlayer::GetFrame() { return static_cast<unsigned int>(myTime * myFPS); }

		bool IsValid() const { return myModel ? true : false; }
	};
}

