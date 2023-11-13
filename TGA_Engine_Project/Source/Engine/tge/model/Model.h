#pragma once
#include <string>
#include <vector>
#include <unordered_map>

#include <tge/Animation/Skeleton.h>
#include <tge/Math/Vector.h>
#include <tge/Math/Transform.h>

struct ID3D11Buffer;

namespace Tga
{

class TextureResource;

struct BoxSphereBounds
{
	// The radius of the Sphere
	float Radius;
	// The extents of the Box
	Vector3f BoxExtents;
	// The local-space center of the shape.
	Vector3f Center;
};

class Model
{
public:

	friend class ModelFactory;

	struct MeshData
	{
		std::string Name;
		std::string MaterialName;
		uint32_t NumberOfVertices;
		uint32_t NumberOfIndices;
		uint32_t Stride;
		uint32_t Offset;
		ID3D11Buffer* VertexBuffer;
		ID3D11Buffer* IndexBuffer;
		BoxSphereBounds Bounds;
	};
		
	void Init(MeshData& aMeshData, const std::wstring& aPath);
	void Init(std::vector<MeshData>& someMeshData, const std::wstring& aPath);

	const char* GetMaterialName(int meshIndex) const { return myMeshData[meshIndex].MaterialName.c_str(); }
	const char* GetMeshName(int meshIndex) const { return myMeshData[meshIndex].Name.c_str(); }

	size_t GetMeshCount() const {return myMeshData.size();}
	MeshData const& GetMeshData(unsigned int anIndex) { return myMeshData[anIndex]; }
	const std::vector<MeshData>& GetMeshDataList() const { return myMeshData; }

	const std::wstring& GetPath() { return myPath; }
	const Skeleton* GetSkeleton() const { return &mySkeleton; }

private:

	Skeleton mySkeleton;
	std::vector<MeshData> myMeshData;
	std::wstring myPath;
};

} // namespace Tga