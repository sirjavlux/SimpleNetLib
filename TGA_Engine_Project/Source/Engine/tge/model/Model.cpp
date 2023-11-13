#include "stdafx.h"
#include <tge/model/Model.h>

using namespace Tga;

void Model::Init(MeshData& aMeshData, const std::wstring& aPath)
{
	myMeshData.push_back(aMeshData);
	myPath = aPath;
}

void Model::Init(std::vector<MeshData>& someMeshData, const std::wstring& aPath)
{
	assert(someMeshData.size() <= MAX_MESHES_PER_MODEL);

	myMeshData = someMeshData;
	myPath = aPath;
}
