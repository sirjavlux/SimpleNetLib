#pragma once
#include <map>
#include <string>
#include <vector>
#include <tge/animation/animation.h>
#include <tge/graphics/Vertex.h>
#include <tge/model/model.h>
#include <tge/model/AnimatedModelInstance.h>

#include "ModelInstancer.h"

struct ID3D11Device;
struct BoxSphereBounds;
namespace Tga
{

class Texture;
class AnimatedModel;

class ModelInstance;
class Model;

class ModelFactory
{
	bool InitUnitCube();
	bool InitUnitPlane();
	bool InitPrimitives();
private:
	ModelFactory();
	~ModelFactory();
public:

	static ModelFactory& GetInstance() { if (!myInstance) { myInstance = new ModelFactory(); } return *myInstance; }
	static void DestroyInstance() { if (myInstance) { delete myInstance; myInstance = nullptr; } }

	std::shared_ptr<Model> GetModel(const std::wstring& someFilePath);
	AnimatedModelInstance GetAnimatedModelInstance(const std::wstring& someFilePath);
	ModelInstance GetModelInstance(const std::wstring& someFilePath);

	std::shared_ptr<Animation> GetAnimation(const std::wstring& someFilePath, const std::shared_ptr<Model>& aModel);
	AnimationPlayer GetAnimationPlayer(const std::wstring& someFilePath, const std::shared_ptr<Model>& aModel);

	ModelInstancer GetModelInstancer(const std::wstring& someFilePath);

	ModelInstance GetUnitCube();
	ModelInstance GetUnitPlane();
	bool ModelHasMesh(const std::wstring& someFilePath);
protected:
	
	std::shared_ptr<Model> LoadModel(const std::wstring& someFilePath);
	Tga::BoxSphereBounds CalculateBoxSphereBounds(std::vector<Tga::Vertex> somePositions);
private:	
	struct AnimationIdentifer
	{
		std::wstring Path;
		std::shared_ptr<Model> Model;

		bool operator==(const AnimationIdentifer& other) const
		{
			return Path == other.Path && Model == other.Model;
		};
	};
	struct AnimationIdentiferHash
	{
		std::size_t operator()(const AnimationIdentifer& identifier) const
		{
			return (std::hash<std::wstring>()(identifier.Path)) * 31 + std::hash<std::shared_ptr<Model>>()(identifier.Model);
		}
	};
	std::unordered_map<std::wstring, std::shared_ptr<Model>> myLoadedModels;	
	std::unordered_map<AnimationIdentifer, std::shared_ptr<Animation>, AnimationIdentiferHash> myLoadedAnimations;

	static ModelFactory* myInstance;
};

} // namespace Tga
