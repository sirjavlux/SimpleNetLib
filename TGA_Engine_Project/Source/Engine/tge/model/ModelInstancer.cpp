#include "stdafx.h"
#include "ModelInstancer.h"

#include <tge/graphics/DX11.h>
#include <tge/graphics/GraphicsEngine.h>
#include <tge/shaders/InstancedModelShader.h>

ID3D11Buffer* Tga::ModelInstancer::GetInstanceBuffer() const
{
	return myInstanceBuffer;
}

void Tga::ModelInstancer::Init(std::shared_ptr<Model> aModel)
{
	myModel = aModel;
	myBufferNumInstances = 0;
	isDirty = false;
}

bool Tga::ModelInstancer::AddInstance(const Transform& aTransform)
{
	if(myInstances.size() < MAX_MODEL_INSTANCES)
	{
		isDirty = true;
		myInstances.push_back(aTransform);
		return true;
	}

	return false;
}

bool Tga::ModelInstancer::RemoveInstance(unsigned int anIdx)
{
	if(anIdx < static_cast<unsigned int>(myInstances.size()))
	{
		isDirty = true;
		myInstances.erase(myInstances.begin() + anIdx);
		return true;
	}

	return false;
}

void Tga::ModelInstancer::RebuildInstances()
{
	if(isDirty)
	{
		isDirty = false;

		D3D11_BUFFER_DESC instanceBufferDesc;
		instanceBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
		instanceBufferDesc.ByteWidth = sizeof(InstanceBufferData) * static_cast<UINT>(myInstances.size());
		instanceBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		instanceBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		instanceBufferDesc.MiscFlags = 0;
		instanceBufferDesc.StructureByteStride = 0;

		std::vector<Matrix4x4f> instanceMatrices;
		instanceMatrices.resize(myInstances.size());
		for(size_t i = 0; i < myInstances.size(); i++)
		{
			instanceMatrices[i] = myInstances[i].GetMatrix();
		}

		D3D11_SUBRESOURCE_DATA instanceData;
		instanceData.pSysMem = &instanceMatrices[0];
		instanceData.SysMemPitch = 0;
		instanceData.SysMemSlicePitch = 0;

		const HRESULT result = DX11::Device->CreateBuffer(&instanceBufferDesc, &instanceData, &myInstanceBuffer);
		assert(!FAILED(result));

		myBufferNumInstances = static_cast<unsigned int>(myInstances.size());
	}
}

void Tga::ModelInstancer::Render(InstancedModelShader& aShader) const
{
	const std::vector<Model::MeshData>& meshData = myModel->GetMeshDataList();
	const size_t meshCount = myModel->GetMeshCount();

	Matrix4x4f Identity = Matrix4x4f::CreateIdentityMatrix();
	for (size_t j = 0; j < meshCount; j++)
	{
		aShader.Render(*this, myTextures[j], meshData[j], Identity, *this);
	}
}
