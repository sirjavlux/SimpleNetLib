#pragma once
#include <tge/math/Transform.h>
#include <tge/model/Model.h>

namespace Tga
{
	class ModelInstance;

	class ModelInstancer : public RenderObjectSharedData
	{
		friend class InstancedModelShader;

		ID3D11Buffer* myInstanceBuffer;
		std::vector<Transform> myInstances;
		std::shared_ptr<Model> myModel;
		unsigned int myBufferNumInstances;

		struct InstanceBufferData
		{
			Matrix4x4f myToWorld;
		} myInstanceBufferData;

		const TextureResource* myTextures[MAX_MESHES_PER_MODEL][4] = {};

		ID3D11Buffer* GetInstanceBuffer() const;

		bool isDirty;
		
	public:

		void Init(std::shared_ptr<Model> aModel);

		/**
		 * Adds an instance with the specified transform.
		 * @param aTransform The transform of the new instance.
		 * @returns True if the instance could be added.
		 */
		bool AddInstance(const Transform& aTransform);

		/**
		 * Removes the instance at the specified index.
		 * @param anIdx The index to remove at.
		 * @returns True if the instance was successfully removed.
		 */
		bool RemoveInstance(unsigned int anIdx);

		void RebuildInstances();

		const TextureResource* const* GetTextures(int meshIndex) const { return myTextures[meshIndex]; }
		void SetTexture(int meshIndex, int textureIndex, TextureResource* texture) { myTextures[meshIndex][textureIndex] = texture; }

		void Render(InstancedModelShader& aShader) const;
	};


}
