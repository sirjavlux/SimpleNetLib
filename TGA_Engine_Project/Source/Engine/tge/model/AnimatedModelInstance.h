#pragma once
#include <map>
#include <memory>
#include <tge/math/matrix4x4.h>
#include <tge/math/transform.h>
#include <tge/EngineDefines.h>
#include <tge/model/model.h>
#include <tge/render/RenderCommon.h>

#include <tge/EngineMacros.h>

namespace Tga
{
	class AnimationPlayer;
	class AnimatedModel;
	class ModelShader;
	class TextureResource;

	struct LocalSpacePose;
	struct ModelSpacePose;

	class AnimatedModelInstance : public RenderObjectSharedData
	{
	public:
		AnimatedModelInstance();
		~AnimatedModelInstance();
		void Init(std::shared_ptr<Model> aModel);

		void Render(const ModelShader& shader) const;

		const Transform& GetTransform() const { return myTransform; }
		void SetTransform(const Transform& someTransform);
		void SetRotation(Rotator someRotation);
		void SetLocation(Vector3f someLocation);
		void SetScale(Vector3f someScale);
		bool IsValid() const { return myModel ? true : false; }
		void SetTexture(int meshIndex, int textureIndex, TextureResource* texture) { myTextures[meshIndex][textureIndex] = texture; }

		void SetPose(const LocalSpacePose& pose);
		void SetPose(const ModelSpacePose& pose);
		void SetPose(const AnimationPlayer& animationInstance);
		void ResetPose();

		std::shared_ptr<Model> GetModel() { return myModel; }
		const std::shared_ptr<const Model> GetModel() const { return myModel; }
	private:
		Transform myTransform;
		std::shared_ptr<Model> myModel = nullptr;

		const TextureResource* myTextures[MAX_MESHES_PER_MODEL][4] = {};
		Matrix4x4f myBoneTransforms[MAX_ANIMATION_BONES];
	};

}
