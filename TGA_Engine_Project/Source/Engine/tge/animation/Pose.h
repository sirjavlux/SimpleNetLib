#pragma once

#include <tge/EngineDefines.h>
#include <tge/Math/Matrix.h>

namespace Tga
{
	/// <summary>
	/// Represents a Pose of a model. Each transform is relative to its parent bone. The parent hierarchy is stored in a Skeleton class.
	/// A pose is only valid for models with the same skeleton as the animation it was created with.
	/// </summary>
	struct LocalSpacePose
	{
		Matrix4x4f JointTransforms[MAX_ANIMATION_BONES];
		size_t Count;
	};

	/// <summary>
	/// Represents a Pose of a model. Each transform is relative to the model it is animating.
	/// A pose is only valid for models with the same skeleton as the animation it was created with.
	/// </summary>
	struct ModelSpacePose
	{
		Matrix4x4f JointTransforms[MAX_ANIMATION_BONES];
		size_t Count;
	};
} // namespace Tga