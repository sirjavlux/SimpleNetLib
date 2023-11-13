#include "stdafx.h"

#include <tge/shaders/ModelShader.h>
#include <tge/engine.h>
#include <tge/graphics/GraphicsEngine.h>
#include <tge/graphics/DX11.h>
#include <tge/model/model.h>
#include <tge/texture/texture.h>
#include <tge/light/LightManager.h>
#include <tge/texture/TextureManager.h>

using namespace Tga;

ModelShader::ModelShader(Engine* aEngine)
	:Shader(*aEngine)
{
}


ModelShader::~ModelShader()
{
}

struct CommonBuf
{
	Matrix4x4f myObToWorld;
};

bool Tga::ModelShader::Init()
{
	return Init(L"shaders/model_shader_VS.cso", L"shaders/model_shader_PS.cso");
}

bool  Tga::ModelShader::Init(const wchar_t* aVertexShaderFile, const wchar_t* aPixelShaderFile)
{
	// GPU
	D3D11_BUFFER_DESC matrixVertexBufferDesc;
	matrixVertexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	matrixVertexBufferDesc.ByteWidth = sizeof(Matrix4x4f) * MAX_ANIMATION_BONES;
	matrixVertexBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixVertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	matrixVertexBufferDesc.MiscFlags = 0;
	matrixVertexBufferDesc.StructureByteStride = 0;

	HRESULT result = S_OK;
	result = DX11::Device->CreateBuffer(&matrixVertexBufferDesc, NULL, &myBoneBuffer);

	// GPU
	D3D11_BUFFER_DESC objectBufferDesc;
	objectBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	objectBufferDesc.ByteWidth = sizeof(CommonBuf);
	objectBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	objectBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	objectBufferDesc.MiscFlags = 0;
	objectBufferDesc.StructureByteStride = 0;


	result = S_OK;
	result = DX11::Device->CreateBuffer(&objectBufferDesc, NULL, &myObjectBuffer);

	return Shader::CreateShaders(aVertexShaderFile, aPixelShaderFile, nullptr);
}

void Tga::ModelShader::Render(const RenderObjectSharedData& sharedData, const TextureResource* const* someTextures, const Model::MeshData& aModelData, const Matrix4x4f& aObToWorld, const Matrix4x4f* someBones) const
{
	{
		ID3D11ShaderResourceView* resourceViews[4];
		int i = 0;
		while (i < 4 && someTextures[i] != nullptr)
		{
			resourceViews[i] = someTextures[i]->GetShaderResourceView();
			i++;
		}

		DX11::Context->PSSetShaderResources(1, i, resourceViews);
	}

	if (!myIsReadyToRender)
	{
		return;
	}

	Shader::PrepareRender(sharedData);

	HRESULT result;
	D3D11_MAPPED_SUBRESOURCE mappedVResource;
	result = DX11::Context->Map(myBoneBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedVResource);
	if (FAILED(result))
	{
		//INFO_PRINT("Error in rendering!");
		return;
	}
	char* dataVPtr = (char*)mappedVResource.pData;
	if (someBones)
	{
		memcpy(dataVPtr, someBones, sizeof(Matrix4x4f) * MAX_ANIMATION_BONES);
	}
	
	DX11::Context->Unmap(myBoneBuffer, 0);
	DX11::Context->VSSetConstantBuffers((int)ConstantBufferSlot::Bones, 1, &myBoneBuffer);

	result = DX11::Context->Map(myObjectBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedVResource);
	if (FAILED(result))
	{
		//INFO_PRINT("Error in rendering!");
		return;
	}
	dataVPtr = (char*)mappedVResource.pData;

	CommonBuf* dataPtrCommon;
	dataPtrCommon = (CommonBuf*)mappedVResource.pData;
	dataPtrCommon->myObToWorld =  aObToWorld;

	DX11::Context->Unmap(myObjectBuffer, 0);
	DX11::Context->VSSetConstantBuffers((int)ConstantBufferSlot::Object, 1, &myObjectBuffer);
	DX11::Context->PSSetConstantBuffers((int)ConstantBufferSlot::Object, 1, &myObjectBuffer);

	DX11::Context->IASetIndexBuffer(aModelData.IndexBuffer, DXGI_FORMAT_R32_UINT, 0);
	const unsigned int strides = aModelData.Stride;
	const unsigned int offsets = 0;
	DX11::Context->IASetVertexBuffers(0, 1, &aModelData.VertexBuffer, &strides, &offsets);

	DX11::LogDrawCall();
	DX11::Context->DrawIndexed(aModelData.NumberOfIndices, 0, 0);
}


bool Tga::ModelShader::CreateInputLayout(const std::string& aVS)
{
	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR", 1, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR", 2, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR", 3, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{ "TEXCOORD", 1, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{ "TEXCOORD", 2, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{ "TEXCOORD", 3, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{ "TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{ "BINORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{ "BONES", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "WEIGHTS", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	unsigned int numElements = sizeof(layout) / sizeof(D3D11_INPUT_ELEMENT_DESC);
	HRESULT result = DX11::Device->CreateInputLayout(&layout[0], numElements, aVS.data(), aVS.size(), myLayout.ReleaseAndGetAddressOf());
	if (FAILED(result))
	{
		ERROR_PRINT("%s", "Layout error");
	}
	return true;
}