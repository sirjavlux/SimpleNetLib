#include "stdafx.h"
#include "AnimationPlayer.h"
#include <tge/engine.h>

using namespace Tga;

void AnimationPlayer::Init(const std::shared_ptr<const Animation>& animation, const std::shared_ptr<const Model>& model)
{
	myModel = model;
	myAnimation = animation;
	myFPS = animation->FramesPerSecond;
}

void AnimationPlayer::Update(float aDeltaTime)
{
	if (myState == AnimationState::Playing)
	{
		myTime += aDeltaTime;

		if (myTime >= GetAnimation()->Duration)
		{
			if (myIsLooping)
			{
				while (myTime >= GetAnimation()->Duration)
					myTime -= GetAnimation()->Duration;
			}
			else
			{
				myTime = GetAnimation()->Duration;
				myState = AnimationState::Finished;
			}
		}

		const float frameRate = 1.0f / myFPS;
		const float result = myTime / frameRate;
		const size_t frame = static_cast<size_t>(std::floor(result));// Which frame we're on
		const float delta = result - static_cast<float>(frame); // How far we have progressed to the next frame.

		size_t nextFrame = frame + 1;
		if (myState == AnimationState::Finished)
		{
			nextFrame = frame;
		}
		else if (nextFrame > GetAnimation()->Length)
			nextFrame = 0;

		// Update all animations
		const Skeleton* skeleton = myModel->GetSkeleton();
		for (size_t i = 0; i < skeleton->Joints.size(); i++)
		{
			const Transform& currentFrameJointXform = myAnimation->Frames[frame].LocalTransforms[i];
			Matrix4x4f jointXform = currentFrameJointXform.GetMatrix();
			if (myIsInterpolating)
			{
				const Transform& nextFrameJointXform = myAnimation->Frames[nextFrame].LocalTransforms[i];

				// Interpolate between the frames
				const Vector3f T = Vector3f::Lerp(currentFrameJointXform.GetPosition(), nextFrameJointXform.GetPosition(), delta);
				const Quatf R = Quatf::Slerp(currentFrameJointXform.GetQuaternion(), nextFrameJointXform.GetQuaternion(), delta);
				const Vector3f S = Vector3f::Lerp(currentFrameJointXform.GetScale(), nextFrameJointXform.GetScale(), delta);

				jointXform = Matrix4x4f::CreateScaleMatrix(S) * R.GetRotationMatrix4x4f() * Matrix4x4f::CreateTranslationMatrix(T);
			}

			Matrix4x4f Result = jointXform;
			myLocalSpacePose.JointTransforms[i] = Result;
		}
		myLocalSpacePose.Count = skeleton->Joints.size();
	}
}

void Tga::AnimationPlayer::UpdatePose()
{
	const size_t frame = GetFrame();// Which frame we're on
	// Update all animations
	const Skeleton* skeleton = myModel->GetSkeleton();
	for (size_t i = 0; i < skeleton->Joints.size(); i++)
	{
		const Transform& currentFrameJointXform = myAnimation->Frames[frame].LocalTransforms[i];
		Matrix4x4f Result = currentFrameJointXform.GetMatrix();
		myLocalSpacePose.JointTransforms[i] = Result;
	}
	myLocalSpacePose.Count = skeleton->Joints.size();
}
