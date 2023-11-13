#include "stdafx.h"
#include "ModelFactory.h"

#include <fstream>

#include <tge/animation/animationPlayer.h>
#include <tge/graphics/DX11.h>
#include <tge/util/StringCast.h>
#include <tge/model/Model.h>
#include <tge/model/ModelInstance.h>
#include <tge/graphics/Vertex.h>
#include <tge/math/matrix4x4.h>
#include <tge/texture/texture.h>
#include <tge/texture/TextureManager.h>

#include <TGAFBXImporter/source/FBXImporter.h>
#include <DDSTextureLoader/DDSTextureLoader11.h>

using namespace Tga;
ModelFactory* ModelFactory::myInstance = nullptr;

#define TEXTURE_SET_0 0
#define TEXTURE_SET_1 1
#define TEXTURE_SET_2 2
#define TEXTURE_SET_3 3

#define VERTEX_COLOR_SET_0 0
#define VERTEX_COLOR_SET_1 1
#define VERTEX_COLOR_SET_2 2
#define VERTEX_COLOR_SET_3 3

#define NUM_BONES_PER_VERTEX 4

using namespace Tga;

//Matrix4x4f ConvertToEngineMatrix33(const aiMatrix3x3& AssimpMatrix)
//{
//    Matrix4x4f mat;
//    for (int i = 0; i < 3; i++)
//    {
//        for (int j = 0; j < 3; j++)
//        {
//            mat(1 + i, 1 + j) = AssimpMatrix[i][j];
//        }
//    }
//
//    return mat;
//}
//
//// constructor from Assimp matrix
//Matrix4x4f ConvertToEngineMatrix4x4f(const aiMatrix4x4& AssimpMatrix)
//{
//    Matrix4x4f mat;
//    for (int i = 0; i < 4; i++)
//    {
//        for (int j = 0; j < 4; j++)
//        {
//            mat(1 + i, 1 + j) = AssimpMatrix[i][j];
//        }
//    }
//    return mat;
//}
//
//inline bool operator <(const aiString& aLhs, const aiString& aRhs)
//{
//    return strcmp(aLhs.data, aRhs.data) < 0;
//}

struct VertexBoneData
{
    unsigned int IDs[NUM_BONES_PER_VERTEX];
    float Weights[NUM_BONES_PER_VERTEX];

    VertexBoneData()
    {
        Reset();
    };

    void Reset()
    {
        memset(IDs, 0, sizeof(IDs));
        memset(Weights, 0, sizeof(Weights));
    }

    void AddBoneData(unsigned int BoneID, float Weight)
    {
        for (unsigned int i = 0; i < sizeof(IDs) / sizeof(IDs[0]); i++) {
            if (Weights[i] == 0.0) {
                IDs[i] = BoneID;
                Weights[i] = Weight;
                return;
            }
        }

        // should never get here - more bones than we have space for
        //assert(0);
    }
};

bool ModelFactory::InitUnitCube()
{
    // First we make a cube.

	// Watch the winding! DX defaults to Clockwise.
	// Assume the winding as if you're viewing the face head on.
	// +Y up, +X right, +Z Forward
    std::vector<Vertex> mdlVertices = {

    	// Front
        {
        	50.0f, -50.0f, 50.0f,
            0, 0, 1,
            1, 0, 0,
            0, -1, 0,
        	1, 1, 1, 1,
        	0, 1
        },
        {
        	50.0f, 50.0f, 50.0f,
            0, 0, 1,
            1, 0, 0,
            0, -1, 0,
        	1, 1, 1, 1,
        	0, 0
        },
        {
        	-50.0f, 50.0f, 50.0f,
            0, 0, 1,
            1, 0, 0,
            0, -1, 0,
        	1, 1, 1, 1,
        	1, 0
        },
        {
        	-50.0f, -50.0f, 50.0f,
            0, 0, 1,
            1, 0, 0,
            0, -1, 0,
        	1, 1, 1, 1,
        	1, 1
        },

    	// Left
        {
        	-50.0f, -50.0f, 50.0f,
            -1, 0, 0,
            0, 0, 1,
            0, -1, 0,
        	1, 0, 0, 1,
        	0, 1
        },
        {
        	-50.0f, 50.0f, 50.0f,
            -1, 0, 0,
            0, 0, 1,
            0, -1, 0,
        	1, 0, 0, 1,
        	0, 0
        },       
        {
        	-50.0f, 50.0f, -50.0f,
            -1, 0, 0,
            0, 0, 1,
            0, -1, 0,
        	1, 0, 0, 1,
        	1, 0
        },
        {
        	-50.0f, -50.0f, -50.0f,
            -1, 0, 0,
            0, 0, 1,
            0, -1, 0,
        	1, 0, 0, 1,
        	1, 1
        },      

    	// Back
        {
        	-50.0f, -50.0f, -50.0f,
            0, 0, -1,
            1, 0, 0,
            0, -1, 0,
        	0, 1, 0, 1,
        	0, 1
        },
        {
        	-50.0f, 50.0f, -50.0f,
            0, 0, -1,
            1, 0, 0,
            0, -1, 0,
        	0, 1, 0, 1,
        	0, 0
        },      
        {
        	50.0f, 50.0f, -50.0f,
            0, 0, -1,
            1, 0, 0,
            0, -1, 0,
        	0, 1, 0, 1,
        	1, 0
        },
    	{
    		50.0f, -50.0f, -50.0f,
            0, 0, -1,
            1, 0, 0,
            0, -1, 0,
    		0, 1, 0, 1,
    		1, 1
    	},       

    	// Right
        {
        	50.0f, -50.0f, -50.0f,
            1, 0, 0,
            0, 0, -1,
            0, -1, 0,
        	0, 0, 1, 1,
        	0, 1
        },
        {
        	50.0f, 50.0f, -50.0f,
            1, 0, 0,
            0, 0, -1,
            0, -1, 0,
        	0, 0, 1, 1,
        	0, 0
        },
        {
        	50.0f, 50.0f, 50.0f,
            1, 0, 0,
            0, 0, -1,
            0, -1, 0,
        	0, 0, 1, 1
        	, 1, 0
        },
        {
        	50.0f, -50.0f, 50.0f,
            1, 0, 0,
            0, 0, -1,
            0, -1, 0,
        	0, 0, 1, 1,
        	1, 1
        },       

    	// Top
        {
        	50.0f, 50.0f, 50.0f,
        	0, 1, 0,
        	1, 0, 0,
        	0, 0, 1,
        	1, 1, 0, 1,
        	0, 1
        },
        {
        	50.0f, 50.0f, -50.0f,
        	0, 1, 0,
        	1, 0, 0,
        	0, 0, 1,
        	1, 1, 0, 1,
        	0, 0
        },
        {
        	-50.0f, 50.0f, -50.0f,
        	0, 1, 0,
        	1, 0, 0,
        	0, 0, 1,
        	1, 1, 0, 1,
        	1, 0
        },
        {
        	-50.0f, 50.0f, 50.0f,
        	0, 1, 0,
        	1, 0, 0,
        	0, 0, 1,
        	1, 1, 0, 1,
        	1, 1
        },

    	// Bottom
        {
        	-50.0f, -50.0f, 50.0f,
        	0, -1, 0,
        	1, 0, 0,
        	0, 0, 1,
        	1, 0, 1, 1,
        	0, 1
        },
        {
        	-50.0f, -50.0f, -50.0f,
        	0, -1, 0,
        	1, 0, 0,
        	0, 0, 1,
        	1, 0, 1, 1,
        	0, 0
        },
        {
        	50.0f, -50.0f, -50.0f,
        	0, -1, 0,
        	1, 0, 0,
        	0, 0, 1,
        	1, 0, 1, 1,
        	1, 0
        },
        {
        	50.0f, -50.0f, 50.0f,
        	0, -1, 0,
        	1, 0, 0,
        	0, 0, 1,
        	1, 0, 1, 1
        	, 1, 1
        },
    };

    std::vector<unsigned int> mdlIndices =
    {
    	0, 1, 2,        /* |/ */
    	0, 2, 3,        /* /| */
    	4, 5, 6,        /* |/ */
    	4, 6, 7,        /* /| */
    	8, 9, 10,       /* |/ */
    	8, 10, 11,      /* /| */
    	12, 13, 14,     /* |/ */
    	12, 14, 15,     /* /| */
    	16, 17, 18,     /* |/ */
    	16, 18, 19,     /* /| */
    	20, 21, 22,     /* |/ */
    	20, 22, 23      /* /| */
    };

    //const Vector3f extentsCenter = 0.5f * (minExtents + maxExtents);
    //const Vector3f boxExtents = 0.5f * (maxExtents - minExtents);
    //const float myBoxSphereRadius = FMath::Max(boxExtents.X, FMath::Max(boxExtents.Y, boxExtents.Z));

    HRESULT result;

    D3D11_BUFFER_DESC vertexBufferDesc{};
    vertexBufferDesc.ByteWidth = static_cast<UINT>(mdlVertices.size()) * static_cast<UINT>(sizeof(Vertex));
    vertexBufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
    vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

    D3D11_SUBRESOURCE_DATA vertexSubresourceData{};
    vertexSubresourceData.pSysMem = &mdlVertices[0];

    ID3D11Buffer* vertexBuffer;
    result = DX11::Device->CreateBuffer(&vertexBufferDesc, &vertexSubresourceData, &vertexBuffer);
    if (FAILED(result))
    {
        return false;
    }

    D3D11_BUFFER_DESC indexBufferDesc{};
    indexBufferDesc.ByteWidth = static_cast<UINT>(mdlIndices.size()) * static_cast<UINT>(sizeof(unsigned int));
    indexBufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
    indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;

    D3D11_SUBRESOURCE_DATA indexSubresourceData{};
    indexSubresourceData.pSysMem = &mdlIndices[0];

    ID3D11Buffer* indexBuffer;
    result = DX11::Device->CreateBuffer(&indexBufferDesc, &indexSubresourceData, &indexBuffer);
    if (FAILED(result))
    {
        return false;
    }

    std::shared_ptr<Model> model = std::make_shared<Model>();

    Model::MeshData meshData = {};
    meshData.NumberOfVertices = static_cast<UINT>(mdlVertices.size());
    meshData.NumberOfIndices = static_cast<UINT>(mdlIndices.size());
    meshData.Stride = sizeof(Vertex);
    meshData.Offset = 0;
    meshData.VertexBuffer = vertexBuffer;
    meshData.IndexBuffer = indexBuffer;
    meshData.Bounds = CalculateBoxSphereBounds(mdlVertices);
    model->Init(meshData, L"Cube");
    myLoadedModels.insert(std::pair<std::wstring, std::shared_ptr<Model>>(L"Cube", model));

    return true;
}

bool ModelFactory::InitUnitPlane()
{
    std::vector<Vertex> mdlVertices;
    std::vector<unsigned int> mdlIndices;

    mdlVertices.push_back({
	    -50.0f, 0.0f, 50.0f,
	    0, 1, 0,
	    1, 0, 0,
	    0, 0, 1,
	    1, 1, 1, 1,
	    0, 0
        });

    mdlVertices.push_back({
	    50.0f, 0.0f, 50.0f,
        0, 1, 0,
        1, 0, 0,
        0, 0, 1,
	    1, 1, 1, 1,
	    1, 0
        });

    mdlVertices.push_back({
		50.0f, 0.0f, -50.0f,
        0, 1, 0,
        1, 0, 0,
        0, 0, 1,
		1, 1, 1, 1,
		1, 1
		});

    mdlVertices.push_back({
	    -50.0f, 0.0f, -50.0f,
        0, 1, 0,
        1, 0, 0,
        0, 0, 1,
	    1, 1, 1, 1,
	    0, 1
        });


    mdlIndices = { 0, 1, 2, 0, 2, 3 };

    //const Vector3f extentsCenter = 0.5f * (minExtents + maxExtents);
    //const Vector3f boxExtents = 0.5f * (maxExtents - minExtents);
    //const float myBoxSphereRadius = FMath::Max(boxExtents.X, FMath::Max(boxExtents.Y, boxExtents.Z));

    HRESULT result;

    D3D11_BUFFER_DESC vertexBufferDesc{};
    vertexBufferDesc.ByteWidth = static_cast<UINT>(mdlVertices.size()) * static_cast<UINT>(sizeof(Vertex));
    vertexBufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
    vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

    D3D11_SUBRESOURCE_DATA vertexSubresourceData{};
    vertexSubresourceData.pSysMem = &mdlVertices[0];

    ID3D11Buffer* vertexBuffer;
    result = DX11::Device->CreateBuffer(&vertexBufferDesc, &vertexSubresourceData, &vertexBuffer);
    if (FAILED(result))
    {
        return false;
    }

    D3D11_BUFFER_DESC indexBufferDesc{};
    indexBufferDesc.ByteWidth = static_cast<UINT>(mdlIndices.size()) * static_cast<UINT>(sizeof(unsigned int));
    indexBufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
    indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;

    D3D11_SUBRESOURCE_DATA indexSubresourceData{};
    indexSubresourceData.pSysMem = &mdlIndices[0];

    ID3D11Buffer* indexBuffer;
    result = DX11::Device->CreateBuffer(&indexBufferDesc, &indexSubresourceData, &indexBuffer);
    if (FAILED(result))
    {
        return false;
    }
	
    std::shared_ptr<Model> model = std::make_shared<Model>();

    Model::MeshData meshData = {};
    meshData.NumberOfVertices = static_cast<UINT>(mdlVertices.size());
    meshData.NumberOfIndices = static_cast<UINT>(mdlIndices.size());
    meshData.Stride = sizeof(Vertex);
    meshData.Offset = 0;
    meshData.VertexBuffer = vertexBuffer;
    meshData.IndexBuffer = indexBuffer;
    meshData.Bounds = CalculateBoxSphereBounds(mdlVertices);
    model->Init(meshData, L"Plane");
    myLoadedModels.insert(std::pair<std::wstring, std::shared_ptr<Model>>(L"Plane", model));
	
    return true;
}

bool ModelFactory::InitPrimitives()
{
    if (!InitUnitCube())
        return false;

    if (!InitUnitPlane())
        return false;

    return true;
}

ModelFactory::ModelFactory()
{
    InitPrimitives();
	myInstance = this;
}

ModelFactory::~ModelFactory()
{
	myInstance = nullptr;
}

template<typename ModelInstanceType>
void AssignMaterials(const std::wstring& someFilePath, Model* Mdl, ModelInstanceType& MI)
{
    std::wstring baseFileName = someFilePath.substr(0, someFilePath.find_last_of(L'.'));
    std::wstring path = someFilePath.substr(0, someFilePath.find_last_of(L"/\\") + 1);

    for (int i = 0; i < Mdl->GetMeshCount(); i++)
    {
        Tga::Engine& engine = *Tga::Engine::GetInstance();

        std::wstring materialFileName = path + string_cast<std::wstring>(Mdl->GetMaterialName(i));

        TextureResource* albedoTexture = nullptr;

        if (albedoTexture == nullptr)
        {
            std::wstring fnAlbedo = materialFileName + L"_C.dds";
            albedoTexture = engine.GetTextureManager().TryGetTexture(fnAlbedo.c_str());
        }

        if (albedoTexture == nullptr)
        {
            std::wstring fnAlbedo = materialFileName + L"_D.dds";
            albedoTexture = engine.GetTextureManager().TryGetTexture(fnAlbedo.c_str());
        }

        if (albedoTexture == nullptr)
        {
            std::wstring fnAlbedo = baseFileName + L"_C.dds";
            albedoTexture = engine.GetTextureManager().TryGetTexture(fnAlbedo.c_str());
        }

        if (albedoTexture == nullptr)
        {
            std::wstring fnAlbedo = baseFileName + L"_D.dds";
            albedoTexture = engine.GetTextureManager().TryGetTexture(fnAlbedo.c_str());
        }

        if (albedoTexture == nullptr)
            albedoTexture = engine.GetTextureManager().GetTexture(L"Textures/T_Default_BC.dds");

        MI.SetTexture(i, 0, albedoTexture);

        TextureResource* normalTexture = nullptr;

        if (normalTexture == nullptr)
        {
            std::wstring fnNormal = materialFileName + L"_N.dds";
            normalTexture = engine.GetTextureManager().TryGetTexture(fnNormal.c_str(), false);
        }

        if (normalTexture == nullptr)
        {
            std::wstring fnNormal = baseFileName + L"_N.dds";
            normalTexture = engine.GetTextureManager().TryGetTexture(fnNormal.c_str(), false);
        }

        if (normalTexture == nullptr)
            normalTexture = engine.GetTextureManager().GetTexture(L"Textures/T_Default_N.dds", false);

        MI.SetTexture(i, 1, normalTexture);

        TextureResource* materialTexture = nullptr;

        if (materialTexture == nullptr)
        {
            std::wstring fnMaterial = materialFileName + L"_M.dds";
            materialTexture = engine.GetTextureManager().TryGetTexture(fnMaterial.c_str(), false);
        }

        if (materialTexture == nullptr)
        {
            std::wstring fnMaterial = baseFileName + L"_M.dds";
            materialTexture = engine.GetTextureManager().TryGetTexture(fnMaterial.c_str(), false);
        }

        if (materialTexture == nullptr)
            materialTexture = engine.GetTextureManager().GetTexture(L"Textures/T_Default_M.dds", false);

        MI.SetTexture(i, 2, materialTexture);
    }
}

ModelInstance ModelFactory::GetModelInstance(const std::wstring& someFilePath)
{
	// This needs to be moved to separate memory structures at some point.
    ModelInstance MI;

    std::shared_ptr<Model> Mdl = GetModel(someFilePath);
    if (!Mdl)
        return MI;

    MI.Init(Mdl);
    
    std::wstring path = someFilePath;
    if (someFilePath.find(L".") != std::string::npos) {
        path = Tga::Settings::ResolveAssetPathW(someFilePath);
    }
    AssignMaterials(path, Mdl.get(), MI);

    return MI;
}

ModelInstancer ModelFactory::GetModelInstancer(const std::wstring& someFilePath)
{
    // This needs to be moved to separate memory structures at some point.
    ModelInstancer MI;

    std::shared_ptr<Model> Mdl = GetModel(someFilePath);
    if (!Mdl)
        return MI;

    MI.Init(Mdl);

    AssignMaterials(someFilePath, Mdl.get(), MI);

    return MI;
}

AnimatedModelInstance ModelFactory::GetAnimatedModelInstance(const std::wstring& someFilePath)
{
    // This needs to be moved to separate memory structures at some point.
    AnimatedModelInstance MI;
    std::wstring resolved_path = Tga::Settings::ResolveAssetPathW(someFilePath);

    std::shared_ptr<Model> Mdl = GetModel(resolved_path);
    if (!Mdl || !Mdl->GetSkeleton()->GetRoot())
        return MI;

    MI.Init(Mdl);

    AssignMaterials(resolved_path, Mdl.get(), MI);

    return MI;
}

ModelInstance ModelFactory::GetUnitCube()
{
    return GetModelInstance(L"Cube");
}

ModelInstance ModelFactory::GetUnitPlane()
{
    return GetModelInstance(L"Plane");
}

bool ModelFactory::ModelHasMesh(const std::wstring& someFilePath)
{
    const std::string ansiFileName = string_cast<std::string>(someFilePath);
    return TGA::FBXImporter::IsValidModelFile(ansiFileName);
}

#pragma optimize("", off)
std::shared_ptr<Model> ModelFactory::LoadModel(const std::wstring& someFilePath)
{
    // The FBX SDK doesn't like widechar :(.
    //const std::string ansiFileName = string_cast<std::string>(someFilePath);
    std::wstring resolved_path = Tga::Settings::ResolveAssetPathW(someFilePath);
    const std::string path = Tga::Settings::ResolveAssetPath(string_cast<std::string>(resolved_path));

    TGA::FBXModel tgaModel;
    if(TGA::FBXImporter::LoadModel(path, tgaModel))
    {
        Skeleton mdlSkeleton;

        if(tgaModel.Skeleton.GetRoot())
        {
            mdlSkeleton.Joints.resize(tgaModel.Skeleton.Bones.size());
            mdlSkeleton.JointNameToIndex.reserve(mdlSkeleton.Joints.size());
            mdlSkeleton.JointName.resize(mdlSkeleton.Joints.size());
            for(size_t j = 0; j < tgaModel.Skeleton.Bones.size(); j++)
            {
	         	Skeleton::Joint& mdlJoint = mdlSkeleton.Joints[j];
                TGA::FBXSkeleton::Bone& tgaJoint = tgaModel.Skeleton.Bones[j];

                Matrix4x4f bindPoseInverseTranspose;
                memcpy(&bindPoseInverseTranspose, &tgaJoint.BindPoseInverse, sizeof(float) * 16);
               
                mdlJoint.BindPoseInverse = Matrix4x4f::Transpose(bindPoseInverseTranspose);
                mdlJoint.Name = tgaJoint.Name;
                mdlJoint.Parent = tgaJoint.Parent;
				mdlJoint.Children = tgaJoint.Children;

                mdlSkeleton.JointNameToIndex.insert({ mdlJoint.Name, j });
                mdlSkeleton.JointName[j] = mdlJoint.Name;
            }
        }

	    std::vector<Model::MeshData> mdlMeshData;
        mdlMeshData.resize(tgaModel.Meshes.size());

        // Convert model to our own format.
        for(size_t i = 0; i < tgaModel.Meshes.size(); i++)
        {
	        // The imported mesh data.
	        TGA::FBXModel::FBXMesh& mesh = tgaModel.Meshes[i];

            // And where we'll put it in our structures.
            Model::MeshData& meshData = mdlMeshData[i];

            // Convert vertices to your own format
            std::vector<Vertex> mdlVertices;
            mdlVertices.resize(mesh.Vertices.size());

            for(size_t v = 0; v < mesh.Vertices.size(); v++)
            {
                // The most important part, the position!
	            mdlVertices[v].Position = {
	            	mesh.Vertices[v].Position[0],
                    mesh.Vertices[v].Position[1],
                    mesh.Vertices[v].Position[2],
                    mesh.Vertices[v].Position[3]
	            };

                // All four vertex color channels I have.
                for(int vCol = 0; vCol < 4; vCol++)
                {
	                mdlVertices[v].VertexColors[vCol] = {
                        mesh.Vertices[v].VertexColors[vCol][0],
                        mesh.Vertices[v].VertexColors[vCol][1],
                        mesh.Vertices[v].VertexColors[vCol][2],
                        mesh.Vertices[v].VertexColors[vCol][3]
	                };
                }

                mdlVertices[v].Normal = Vector3f(mesh.Vertices[v].Normal[0], mesh.Vertices[v].Normal[1], mesh.Vertices[v].Normal[2]);
                mdlVertices[v].Binormal = Vector3f(mesh.Vertices[v].Binormal[0], mesh.Vertices[v].Binormal[1], mesh.Vertices[v].Binormal[2]);
                mdlVertices[v].Tangent = Vector3f(mesh.Vertices[v].Tangent[0], mesh.Vertices[v].Tangent[1], mesh.Vertices[v].Tangent[2]);

            	for(unsigned int UVch = 0; UVch < 4; UVch++)
                {
                	mdlVertices[v].UVs[UVch] = {
                         mesh.Vertices[v].UVs[UVch][0],
                         mesh.Vertices[v].UVs[UVch][1]
                	};
                }

                mdlVertices[v].Bones = {
            		static_cast<float>(mesh.Vertices[v].BoneIDs[0]),
                    static_cast<float>(mesh.Vertices[v].BoneIDs[1]),
                    static_cast<float>(mesh.Vertices[v].BoneIDs[2]),
            		static_cast<float>(mesh.Vertices[v].BoneIDs[3])
                };

                mdlVertices[v].Weights = {
            		mesh.Vertices[v].BoneWeights[0],
                    mesh.Vertices[v].BoneWeights[1],
                    mesh.Vertices[v].BoneWeights[2],
            		mesh.Vertices[v].BoneWeights[3]
                };
            }

            std::vector<unsigned int>& mdlIndices = mesh.Indices;

            HRESULT result; 
	
	        D3D11_BUFFER_DESC vertexBufferDesc{};
	        vertexBufferDesc.ByteWidth = static_cast<UINT>(mdlVertices.size()) * static_cast<UINT>(sizeof(Vertex));
	        vertexBufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
	        vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

	        D3D11_SUBRESOURCE_DATA vertexSubresourceData{};
	        vertexSubresourceData.pSysMem = &mdlVertices[0];

	        ID3D11Buffer* vertexBuffer;
	        result = DX11::Device->CreateBuffer(&vertexBufferDesc, &vertexSubresourceData, &vertexBuffer);
	        if (FAILED(result))
	        {
	            return nullptr;
	        }

	        D3D11_BUFFER_DESC indexBufferDesc{};
	        indexBufferDesc.ByteWidth = static_cast<UINT>(mdlIndices.size()) * static_cast<UINT>(sizeof(float)); // TODO: What :P Sizeof should be uint.
	        indexBufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
	        indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;

	        D3D11_SUBRESOURCE_DATA indexSubresourceData{};
	        indexSubresourceData.pSysMem = &mdlIndices[0];

            ID3D11Buffer* indexBuffer;
	        result = DX11::Device->CreateBuffer(&indexBufferDesc, &indexSubresourceData, &indexBuffer);
	        if (FAILED(result))
	        {
	            return nullptr;
	        }

	        meshData.NumberOfVertices = static_cast<UINT>(mdlVertices.size());
	        meshData.NumberOfIndices = static_cast<UINT>(mdlIndices.size());
	        meshData.Stride = sizeof(Vertex);
	        meshData.Offset = 0;
	        meshData.VertexBuffer = vertexBuffer;
	        meshData.IndexBuffer = indexBuffer;
	        meshData.Name = mesh.MeshName;
            if (tgaModel.Materials.size() > mesh.MaterialIndex)
            {
                meshData.MaterialName = tgaModel.Materials[mesh.MaterialIndex].MaterialName;
            }
            else
            {
                meshData.MaterialName = "";
            }
            meshData.Bounds = CalculateBoxSphereBounds(mdlVertices);
        }

        std::shared_ptr<Model> model = std::make_shared<Model>();

	    model->Init(mdlMeshData, resolved_path);
        if(mdlSkeleton.Joints.size() > 0)
        {
	        model->mySkeleton = std::move(mdlSkeleton);
        }

	    myLoadedModels.insert(std::pair<std::wstring, std::shared_ptr<Model>>(someFilePath, model));

	    return model;
    }

    return nullptr;
}

Tga::BoxSphereBounds Tga::ModelFactory::CalculateBoxSphereBounds(std::vector<Tga::Vertex> somePositions)
{
    Vector3f minExtents = Vector3f::Zero;
    Vector3f maxExtents = Vector3f::Zero;

    for (unsigned int v = 0; v < somePositions.size(); v++)
    {
        if (somePositions[v].Position.x > maxExtents.X)
            maxExtents.X = somePositions[v].Position.x;
        if (somePositions[v].Position.y > maxExtents.Y)
            maxExtents.Y = somePositions[v].Position.y;
        if (somePositions[v].Position.z > maxExtents.Z)
            maxExtents.Z = somePositions[v].Position.z;

        if (somePositions[v].Position.x < minExtents.X)
            minExtents.X = somePositions[v].Position.x;
        if (somePositions[v].Position.y < minExtents.Y)
            minExtents.Y = somePositions[v].Position.y;
        if (somePositions[v].Position.z < minExtents.Z)
            minExtents.Z = somePositions[v].Position.z;
    }

    const Vector3f extentsCenter = 0.5f * (minExtents + maxExtents);
    const Vector3f boxExtents = 0.5f * (maxExtents - minExtents);
    const float myBoxSphereRadius = FMath::Max(boxExtents.X, FMath::Max(boxExtents.Y, boxExtents.Z));
    return { myBoxSphereRadius, boxExtents, extentsCenter };
}

std::shared_ptr<Animation> ModelFactory::GetAnimation(const std::wstring& someFilePath, const std::shared_ptr<Model>& aModel)
{
	// The FBX SDK doesn't like widechar :(.
    const std::string ansiFileName = string_cast<std::string>(someFilePath);
    TGA::FBXAnimation fbxAnimation;

    auto it = myLoadedAnimations.find(AnimationIdentifer{ someFilePath , aModel });
    if (it != myLoadedAnimations.end())
        return it->second;

    if (TGA::FBXImporter::LoadAnimation(ansiFileName, aModel->GetSkeleton()->JointName, fbxAnimation))
    {
        std::shared_ptr<Animation> animation = std::make_shared<Animation>();
        animation->Name = string_cast<std::wstring>(fbxAnimation.Name);
        animation->Length = fbxAnimation.Length;
        animation->FramesPerSecond = fbxAnimation.FramesPerSecond;
        animation->Frames.resize(fbxAnimation.Frames.size());
        animation->Duration = static_cast<float>(fbxAnimation.Duration);

        for (size_t f = 0; f < animation->Frames.size(); f++)
        {
            animation->Frames[f].LocalTransforms.resize(fbxAnimation.Frames[f].LocalTransforms.size());

            for (size_t t = 0; t < fbxAnimation.Frames[f].LocalTransforms.size(); t++)
            {
                Matrix4x4f localMatrix;
                memcpy(&localMatrix, &fbxAnimation.Frames[f].LocalTransforms[t], sizeof(float) * 16);

                Vector3f T, R, S;
                localMatrix.DecomposeMatrix(T, R, S);
                Quatf Rot(localMatrix);

                animation->Frames[f].LocalTransforms[t] = { T, Rot, S };
            }
        }

        myLoadedAnimations[AnimationIdentifer{ someFilePath , aModel }] = animation;

        return animation;
    }

    return nullptr;
}
#pragma optimize("", on)

AnimationPlayer ModelFactory::GetAnimationPlayer(const std::wstring& someFilePath, const std::shared_ptr<Model>& aModel)
{
    AnimationPlayer instance;
    std::shared_ptr<Animation> animation = GetAnimation(Tga::Settings::ResolveAssetPathW(someFilePath), aModel);
    if (animation)
        instance.Init(animation, aModel);

    return instance;
}

std::shared_ptr<Model> ModelFactory::GetModel(const std::wstring& someFilePath)
{
    auto it = myLoadedModels.find(someFilePath);
    if (it != myLoadedModels.end())
        return it->second;
	
    return LoadModel(someFilePath);
}