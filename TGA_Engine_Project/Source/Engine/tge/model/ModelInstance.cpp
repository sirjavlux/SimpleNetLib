#include "stdafx.h"
#include <tge/model/ModelInstance.h>
#include <tge/model/Model.h>
#include <tge/shaders/ModelShader.h>

using namespace Tga;

void ModelInstance::Init(std::shared_ptr<Model> aModel)
{
	myModel = aModel;
}

Model* ModelInstance::GetModel() const
{
	return myModel.get();
}

void ModelInstance::SetTransform(const Transform& someTransform)
{
	myTransform = someTransform;
}

void ModelInstance::SetRotation(Rotator someRotation)
{
	// Really should unroll rotations as well somewhere
	// so we can use -180 to 180 instead of 0 to 360.
	myTransform.SetRotation(someRotation);
}

void ModelInstance::SetLocation(Vector3f someLocation)
{
	myTransform.SetPosition(someLocation);
}

void ModelInstance::SetScale(Vector3f someScale)
{
	myTransform.SetScale(someScale);
}

void ModelInstance::Render(const ModelShader& shader) const
{
	const std::vector<Model::MeshData>& meshData = myModel->GetMeshDataList();

	for (int j = 0; j < meshData.size(); j++)
	{
		shader.Render(*this, myTextures[j], meshData[j], myTransform.GetMatrix());
	}
}

