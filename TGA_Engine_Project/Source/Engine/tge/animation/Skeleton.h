#pragma once
#include <string>
#include <vector>
#include <unordered_map>

#include <tge/Math/Vector.h>
#include <tge/Math/Transform.h>

namespace Tga
{

struct LocalSpacePose;
struct ModelSpacePose;

/// <summary>
/// Holds the Skeleton for a model and useful metadata.
/// Can be used to lookup joints from names and hierarchical information for joints.
/// </summary>
struct Skeleton
{
	std::string Name;

	struct Joint
	{
		Matrix4x4f BindPoseInverse;
		int Parent;
		std::vector<unsigned int> Children;
		std::string Name;

		bool operator==(const Joint& aJoint) const
		{
			const bool A = BindPoseInverse == aJoint.BindPoseInverse;
			const bool B = Parent == aJoint.Parent;
			const bool C = Name == aJoint.Name;
			const bool D = Children == aJoint.Children;

			return A && B && C && D;
		}

		Joint()
			: BindPoseInverse{ 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1 }, Parent(-1)
		{
		}
	};

	std::vector<Joint> Joints;
	std::unordered_map<std::string, size_t> JointNameToIndex;
	std::vector<std::string> JointName;

	std::unordered_map<std::wstring, size_t> myAnimationNameToIndex;

	const Joint* GetRoot() const { if (Joints.empty()) return nullptr; return &Joints[0]; }

	bool operator==(const Skeleton& aSkeleton) const
	{
		return Joints == aSkeleton.Joints;
	}

	void ConvertPoseToModelSpace(const LocalSpacePose& in, ModelSpacePose& out) const;
	void ApplyBindPoseInverse(const ModelSpacePose& in, Matrix4x4f* out) const;
private:
	void ConvertPoseToModelSpace(const LocalSpacePose& in, ModelSpacePose& out, unsigned aBoneIdx, const Matrix4x4f& aParentTransform)  const;
};

} // namespace Tga