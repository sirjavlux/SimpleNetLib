#include "stdafx.h"
#include "AnimatedModelInstance.h"
#include <tge/model/ModelFactory.h>
#include <tge/animation/Animation.h>
#include <tge/animation/AnimationPlayer.h>
#include <tge/animation/Pose.h>
#include <tge/drawers/DebugDrawer.h>
#include <tge/shaders/ModelShader.h>

using namespace Tga;

AnimatedModelInstance::AnimatedModelInstance()
{

}

AnimatedModelInstance::~AnimatedModelInstance()
{
	
}

void AnimatedModelInstance::Init(std::shared_ptr<Model> aModel)
{
	myModel = aModel;
}

void AnimatedModelInstance::SetTransform(const Transform& someTransform)
{
	myTransform = someTransform;
}

void AnimatedModelInstance::SetRotation(Rotator someRotation)
{
	// Really should unroll rotations as well somewhere
	// so we can use -180 to 180 instead of 0 to 360.
	myTransform.SetRotation(someRotation);
}

void AnimatedModelInstance::SetLocation(Vector3f someLocation)
{
	myTransform.SetPosition(someLocation);
}

void AnimatedModelInstance::SetScale(Vector3f someScale)
{
	myTransform.SetScale(someScale);
}

void AnimatedModelInstance::Render(const ModelShader& shader) const
{
	const std::vector<Model::MeshData>& meshData = myModel->GetMeshDataList();

	for (int j = 0; j < meshData.size(); j++)
	{
		shader.Render(*this, myTextures[j], meshData[j], myTransform.GetMatrix(), myBoneTransforms);
	}
}

void AnimatedModelInstance::SetPose(const LocalSpacePose& pose)
{
	ModelSpacePose modelSpacePose;
	myModel->GetSkeleton()->ConvertPoseToModelSpace(pose, modelSpacePose);

	SetPose(modelSpacePose);
}

void AnimatedModelInstance::SetPose(const ModelSpacePose& pose)
{
	myModel->GetSkeleton()->ApplyBindPoseInverse(pose, myBoneTransforms);
}

void AnimatedModelInstance::SetPose(const AnimationPlayer& animationInstance)
{
	SetPose(animationInstance.GetLocalSpacePose());
}

void AnimatedModelInstance::ResetPose()
{
	for (int i = 0; i < MAX_ANIMATION_BONES; i++)
	{
		myBoneTransforms[i] = Matrix4x4f::CreateIdentityMatrix();
	}
}
