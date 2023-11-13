#pragma once
#include <string>
#include <unordered_map>
#include <vector>

namespace TGA
{
	// Extremely barebones container for a 4x4 float matrix.
	struct Matrix
	{
		float Data[16];

		bool operator==(const Matrix& other) const
		{
			return Data == other.Data;
		}
	};

	struct FBXVertex
	{
		float Position[4] = {0,0,0,1};
		float VertexColors[4][4]
		{
			{0, 0, 0, 0},
			{0, 0, 0, 0},
			{0, 0, 0, 0},
			{0, 0, 0, 0},
		};

		float UVs[4][2]
		{
			{0, 0},
			{0, 0},
			{0, 0},
			{0, 0}
		};

		float Normal[3] = {0, 0, 0};
		float Tangent[3] = { 0, 0, 0 };
		float Binormal[3] = { 0, 0, 0 };

		unsigned int BoneIDs[4] = {0, 0, 0, 0};
		float BoneWeights[4] = {0, 0, 0, 0};

		FBXVertex() = default;

		FBXVertex(float X, float Y, float Z, float R, float G, float B, float A, float U, float V)
		{
			Position[0] = X;
			Position[1] = Y;
			Position[2] = Z;

			VertexColors[0][0] = R;
			VertexColors[0][1] = G;
			VertexColors[0][2] = B;
			VertexColors[0][3] = A;

			UVs[0][0] = U;
			UVs[0][1] = V;
		}

		FBXVertex(float X, float Y, float Z, float nX, float nY, float nZ, float tX, float tY, float tZ, float bX, float bY, float bZ, float R, float G, float B, float A, float U, float V)
		{
			Position[0] = X;
			Position[1] = Y;
			Position[2] = Z;

			VertexColors[0][0] = R;
			VertexColors[0][1] = G;
			VertexColors[0][2] = B;
			VertexColors[0][3] = A;

			UVs[0][0] = U;
			UVs[0][1] = V;

			Normal[0] = nX;
			Normal[1] = nY;
			Normal[2] = nZ;

			Tangent[0] = tX;
			Tangent[1] = tY;
			Tangent[2] = tZ;

			Binormal[0] = bX;
			Binormal[1] = bY;
			Binormal[2] = bZ;
		}

		bool operator==(const FBXVertex& other) const
		{
			// A vertex is just a POD object so we can do this.
			return memcmp(this, &other, sizeof(FBXVertex)) == 0;
		}
	};

	struct FBXSkeleton
	{
		std::string Name;

		// The structure of a joint/bone.
		struct Bone
		{
			Matrix BindPoseInverse;
			int Parent;
			std::vector<unsigned int> Children;
			std::string Name;

			bool operator==(const Bone& aBone) const
			{
				const bool A = BindPoseInverse == aBone.BindPoseInverse;
				const bool B = Parent == aBone.Parent;
				const bool C = Name == aBone.Name;
				const bool D = Children == aBone.Children;

				return A && B && C && D;
			}

			Bone()
				: BindPoseInverse{1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1}, Parent(-1)
			{
			}
		};

		// All our joints as an indexed map.
		std::vector<Bone> Bones;

		// Acceleration map to find joint index from name.
		std::unordered_map<std::string, size_t> BoneNameToIndex;

		const Bone* GetRoot() const { if(!Bones.empty()) { return &Bones[0]; } return nullptr; }

		bool operator==(const FBXSkeleton& aSkeleton) const
		{
			return Bones == aSkeleton.Bones;
		}
	};

	struct FBXTexture
	{
		std::string Name;
		std::string Path;
		std::string RelativePath;
	};

	struct FBXMaterial
	{
		std::string MaterialName;
		FBXTexture Emissive;
		FBXTexture Ambient;
		FBXTexture Diffuse;
		FBXTexture Specular;
		FBXTexture Shininess;
		FBXTexture Bump;
		FBXTexture NormalMap;
		FBXTexture TransparentColor;
		FBXTexture Reflection;
		FBXTexture Displacement;
		FBXTexture VectorDisplacement;
	};

	struct FBXModel
	{
		struct FBXBoxSphereBounds
		{
			float BoxExtents[3];
			float Center[3];
			float Radius;
		};

		struct FBXMesh
		{
			std::vector<FBXVertex> Vertices;
			std::vector<unsigned int> Indices;

			unsigned int MaterialIndex;
			std::string MeshName;
		};

		FBXSkeleton Skeleton;

		std::vector<FBXMesh> Meshes;
		std::vector<FBXMaterial> Materials;
		std::string Name;
		FBXBoxSphereBounds BoxSphereBounds;
	};

	struct FBXAnimation
	{
		struct Frame
		{
			std::vector<Matrix> GlobalTransforms;
			std::vector<Matrix> LocalTransforms;
		};

		// The animation frames.
		std::vector<Frame> Frames;

		// How long this animation is in frames.
		unsigned int Length;

		// The duration of this animation.
		double Duration;

		// The FPS of this animation.
		float FramesPerSecond;

		std::string Name;
	};
}
