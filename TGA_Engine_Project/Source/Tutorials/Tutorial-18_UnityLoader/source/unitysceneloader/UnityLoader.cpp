#include <stdafx.h>

#include "UnityLoader.h"

#include <tge/Model/ModelFactory.h> // ?? used?
#include <tge/Model/ModelInstance.h>
#include <tge/texture/TextureManager.h>
#include <tge/graphics/Camera.h>
#include <tge/settings/settings.h>

#include <functional>
#include <fstream>
#include <streambuf>
#include <assert.h>
#include <tge/util/StringCast.h>

#include <nlohmann/json.hpp>
#include <array>

using namespace Tga;

UnityLoader::UnityLoader(){}

void UnityLoader::LoadModels(std::string aModelPathFile)
{
	std::ifstream modelFile(aModelPathFile);
	std::string s;
	int index = -1;
	while (std::getline(modelFile, s))
	{
		index++;
		std::replace(s.begin(), s.end(), '/', '\\');
		myModelPaths[index] = s;
	}

	myModelPathsFetched = true;
}

// Much faster than ASCII load, but unreadable files, better with ASCII for debugging
std::vector<ObjectData> UnityLoader::LoadGameObjectsBinary(const std::string& aGameObjectFile)
{
	std::ifstream t(aGameObjectFile, std::ios::binary);
	assert(t.is_open());

	std::string str((std::istreambuf_iterator<char>(t)), std::istreambuf_iterator<char>());

	unsigned int count = 0;
	memcpy(&count, &str[0], sizeof(int));

	assert(count != 0);
	char* ptr = &str[0];
	ptr += sizeof(int);

	ObjectDataBin* data = new ObjectDataBin[count];
	memcpy(data, ptr, sizeof(ObjectDataBin) * count);

	std::vector<ObjectData> returnedData;
	for (size_t i=0; i< count; i++)
	{
		returnedData.push_back(ObjectData());
		ObjectData& object = returnedData.back();

		memcpy(&object, &data[i], sizeof(ObjectDataRaw)); // Memcpy all the essential data
		object.myRelativePath = myModelPaths[data[i].myModelIndex];
	}
	delete[] data;
	t.close();
	return returnedData;
}

using namespace nlohmann;
static void TraverseObject(json object, RenderData& someRenderData) {
	ModelFactory& modelFactory = ModelFactory::GetInstance();
	Engine& engine = *Tga::Engine::GetInstance();

	std::string name = object["name"];
	
	// we do this so that we may carry information about our own classification, since not all object will have a type.
	std::string type = "unknown";
	if (object.contains("type")) {
		type = object.at("type");
	}

	Vector3f position = {
		object["position"]["x"],
		object["position"]["y"],
		object["position"]["z"]
	};
	Quaternion rotation(
		(float)object["rotation"]["w"],
		(float)object["rotation"]["x"],
		(float)object["rotation"]["y"],
		(float)object["rotation"]["z"]
	);
	Vector3f scale = {
		object["scale"]["x"],
		object["scale"]["y"],
		object["scale"]["z"],
	};

	if (type == "camera") {
		someRenderData.myMainCamera = std::make_shared<Camera>(Camera());
		Camera& camera = *someRenderData.myMainCamera;
		Vector2ui resolution = Tga::Engine::GetInstance()->GetRenderSize();

		camera.SetPosition(position * 100.0f);
		// rotation?

		camera.SetPerspectiveProjection(
			(float)object["fov"],
			{
				(float)resolution.x,
				(float)resolution.y
			},
			(float)object["near"] * 100.0f, (float)object["far"] * 100.0f
		);
	}
	else if (type == "model" || type == "animated_model")
	{
		std::wstring assetpath = string_cast<std::wstring>((std::string)object["path"]);
		std::shared_ptr<ModelInstance> obj = std::make_shared<ModelInstance>(modelFactory.GetModelInstance(assetpath));

		Transform t(position * 100.0f, rotation, scale);
		obj->SetTransform(t);

		//obj->SetScale({ object["myScaleX"] * 1.0f, object["myScaleY"], object["myScaleZ"] * -1.0f });
		someRenderData.myModels.emplace_back(obj);
	}
	else if (type == "sprite") {
		SpriteSharedData& sprite = someRenderData.mySpriteSharedData;
		
		Sprite3DInstanceData& data = someRenderData.mySpriteInstanceData;

		Vector3f pivot = { object["pivot"]["x"], object["pivot"]["y"], 0.0f};
		pivot.x *= -1.0f;

		std::wstring assetpath = string_cast<std::wstring>((std::string)object["path"]);

		sprite.myTexture = engine.GetTextureManager().GetTexture(assetpath.c_str());
		auto tex_size = sprite.myTexture->CalculateTextureSize();
		Vector2ui resolution = Tga::Engine::GetInstance()->GetRenderSize();

		data = {};

		data.myTransform = Matrix4x4f::CreateIdentityMatrix();
		Matrix3x3f scale_matrix = Matrix4x4f::CreateScaleMatrix({
			tex_size.x * scale.x,
			tex_size.y * scale.y,
			scale.z
		});
		Matrix3x3f rotation_matrix = rotation.GetRotationMatrix4x4f();
				
		Matrix3x3f m = scale_matrix * rotation_matrix;
		Vector3f p = pivot * m + 100.0f * position;

		data.myTransform = Matrix4x4f
		{
			m(1,1), m(1,2), m(1,3), 0,
			m(2,1), m(2,2), m(2,3), 0,
			m(3,1), m(3,2), m(3,3), 0,
			p.x,    p.y,    p.z,    1
		};		
	}

	for (auto& child : object["children"]) {
		TraverseObject(child, someRenderData);
	}
}

void UnityLoader::LoadGameObjectsASCII(
	const std::string& aGameObjectFile,
	RenderData &someRenderData
) {
	std::vector<ObjectData> loadedObjects;
	
	std::ifstream t(Settings::ResolveGameAssetPath(aGameObjectFile));
	assert(t.is_open());

	json j = json::parse(t);
	t.close();

	for (json& object : j["scene_objects"]) {
		TraverseObject(object, someRenderData);
	}
}

