#pragma once
#include <string>
#include <vector>
#include <map>

class ObjectDataRaw
{
public:
	float myRotX;
	float myRotY;
	float myRotZ;
	float myPosX;
	float myPosY;
	float myPosZ;

	float myScaleX;
	float myScaleY;
	float myScaleZ;
};

class ObjectDataBin : public ObjectDataRaw
{
public:
	int myModelIndex;
};
class ObjectData : public ObjectDataRaw
{
public:
	std::string myRelativePath;
};



namespace Tga {
	class RenderTarget;
	class ModelInstance;
	class Camera;
	class UnityLoader;

	struct RenderData
	{
		RenderTarget* myBackBuffer;

		std::vector<std::shared_ptr<ModelInstance>> myModels;
		std::shared_ptr<Camera> myMainCamera;

		Tga::SpriteSharedData mySpriteSharedData;
		Tga::Sprite3DInstanceData mySpriteInstanceData;
		std::unique_ptr<UnityLoader> myUnityLoader;
	};

	class ModelInstance;

	class UnityLoader
	{
	public:
		UnityLoader();
		void LoadModels(std::string aModelPathFile);
		std::vector<ObjectData> LoadGameObjectsBinary(const std::string& aGameObjectFile);
		void LoadGameObjectsASCII(
			const std::string& aGameObjectFile, 
			RenderData &someRenderData
		);
	private:

		std::map<int, std::string> myModelPaths;
		bool myModelPathsFetched = false;
	};
}

