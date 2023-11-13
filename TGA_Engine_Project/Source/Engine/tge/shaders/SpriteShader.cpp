#include "stdafx.h"
#include <tge/graphics/DX11.h>
#include <tge/shaders/SpriteShader.h>
#include <tge/shaders/ShaderCommon.h>
#include <tge/texture/texture.h>
#include <tge/texture/TextureManager.h>
#include <tge/render/RenderObject.h>
#include <d3dcommon.h>
#include <d3d11.h>


Tga::SpriteShader::SpriteShader()
	: Shader(*Engine::GetInstance())
{
	myCurrentDataIndex = -1;
	myBufferIndex = (int)ShaderDataBufferIndex::Index_1;
	myCurrentTextureCount = 0;
}

bool Tga::SpriteShader::Init(const wchar_t* aVertex, const wchar_t* aPixel)
{
	if (!CreateShaders(aVertex, aPixel))
	{
		return false;
	}

	D3D11_BUFFER_DESC commonBufferDesc;
	commonBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	commonBufferDesc.ByteWidth = sizeof(Tga::Vector4f) * 64;
	commonBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	commonBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	commonBufferDesc.MiscFlags = 0;
	commonBufferDesc.StructureByteStride = 0;

	HRESULT result = DX11::Device->CreateBuffer(&commonBufferDesc, NULL, myCustomBuffer.ReleaseAndGetAddressOf());
	if (result != S_OK)
	{
		ERROR_PRINT("DX2D::CCustomShader::PostInit - Size missmatch between CPU and GPU(shader)");
		return false;
	}

	return true;
}

void Tga::SpriteShader::SetDataBufferIndex(ShaderDataBufferIndex aBufferRegisterIndex)
{
	myBufferIndex = (unsigned char)aBufferRegisterIndex;
}

void Tga::SpriteShader::SetShaderdataFloat4(Tga::Vector4f someData, ShaderDataID aID)
{
	if (aID > MAX_SHADER_DATA)
	{
		ERROR_PRINT("DX2D::CCustomShader::SetShaderdataFloat4() The id is bigger than allowed size");
		return;
	}
	myCustomData[aID] = someData;
	if (aID > myCurrentDataIndex)
	{
		myCurrentDataIndex = aID;
	}
}

void Tga::SpriteShader::SetTextureAtRegister(Tga::TextureResource* aTexture, ShaderTextureSlot aRegisterIndex)
{
	myBoundTextures[aRegisterIndex - 4] = BoundTexture(aTexture, (unsigned char)aRegisterIndex);

	if (myCurrentTextureCount < (aRegisterIndex - 4) + 1)
	{
		myCurrentTextureCount = static_cast<unsigned char>((aRegisterIndex - 4) + 1);
	}
}

bool Tga::SpriteShader::PrepareRender(const SpriteSharedData& aSharedData)
{
	if (!Shader::PrepareRender(aSharedData))
		return false;

	Engine& engine = *Engine::GetInstance();

	ID3D11DeviceContext* context = DX11::Context;

	ID3D11ShaderResourceView* textures[1 + ShaderMap::MAP_MAX];

	textures[0] = aSharedData.myTexture ? aSharedData.myTexture->GetShaderResourceView() : engine.GetTextureManager().GetWhiteSquareTexture()->GetShaderResourceView();

	for (unsigned short index = 0; index < ShaderMap::MAP_MAX; index++)
	{
		textures[1 + index] = engine.GetTextureManager().GetDefaultNormalMapResource();
		if (aSharedData.myMaps[index])
		{
			textures[1 + index] = aSharedData.myMaps[index]->GetShaderResourceView();
		}
	}
	DX11::Context->PSSetShaderResources(1, 1 + ShaderMap::MAP_MAX, textures);

	if (myCustomBuffer)
	{
		D3D11_MAPPED_SUBRESOURCE mappedResourceCommon;
		Tga::Vector4f* dataPtrCommon;
		HRESULT result = context->Map(myCustomBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResourceCommon);
		if (FAILED(result))
		{
			return false;
		}

		dataPtrCommon = (Tga::Vector4f*)mappedResourceCommon.pData;
		for (int i = 0; i < myCurrentDataIndex + 1; i++)
		{
			dataPtrCommon[i].x = myCustomData[i].x;
			dataPtrCommon[i].y = myCustomData[i].y;
			dataPtrCommon[i].z = myCustomData[i].z;
			dataPtrCommon[i].w = myCustomData[i].w;
		}

		context->Unmap(myCustomBuffer.Get(), 0);
		context->VSSetConstantBuffers(myBufferIndex, 1, myCustomBuffer.GetAddressOf());
		context->PSSetConstantBuffers(myBufferIndex, 1, myCustomBuffer.GetAddressOf());
	}

	for (int i = 0; i < myCurrentTextureCount; i++)
	{
		ID3D11ShaderResourceView* customTextures[1];
		customTextures[0] = myBoundTextures[i].myTexture->GetShaderResourceView();

		context->PSSetShaderResources(myBoundTextures[i].myIndex, 1, customTextures);
	}

	return true;
}

bool Tga::SpriteShader::CreateInputLayout(const std::string& aVS)
{
	D3D11_INPUT_ELEMENT_DESC polygonLayout[9];

	int i = 0;
	polygonLayout[i].SemanticName = "POSITION";
	polygonLayout[i].SemanticIndex = 0;
	polygonLayout[i].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	polygonLayout[i].InputSlot = 0;
	polygonLayout[i].AlignedByteOffset = 0;
	polygonLayout[i].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[i].InstanceDataStepRate = 0;
	i++;

	polygonLayout[i].SemanticName = "TEXCOORD";
	polygonLayout[i].SemanticIndex = i - 1;
	polygonLayout[i].Format = DXGI_FORMAT_R32G32B32A32_UINT;
	polygonLayout[i].InputSlot = 0;
	polygonLayout[i].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	polygonLayout[i].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[i].InstanceDataStepRate = 0;
	i++;

	polygonLayout[i].SemanticName = "TEXCOORD";
	polygonLayout[i].SemanticIndex = i - 1;
	polygonLayout[i].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	polygonLayout[i].InputSlot = 1;
	polygonLayout[i].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	polygonLayout[i].InputSlotClass = D3D11_INPUT_PER_INSTANCE_DATA;
	polygonLayout[i].InstanceDataStepRate = 1;
	i++;

	polygonLayout[i].SemanticName = "TEXCOORD";
	polygonLayout[i].SemanticIndex = i - 1;
	polygonLayout[i].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	polygonLayout[i].InputSlot = 1;
	polygonLayout[i].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	polygonLayout[i].InputSlotClass = D3D11_INPUT_PER_INSTANCE_DATA;
	polygonLayout[i].InstanceDataStepRate = 1;
	i++;

	polygonLayout[i].SemanticName = "TEXCOORD";
	polygonLayout[i].SemanticIndex = i - 1;
	polygonLayout[i].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	polygonLayout[i].InputSlot = 1;
	polygonLayout[i].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	polygonLayout[i].InputSlotClass = D3D11_INPUT_PER_INSTANCE_DATA;
	polygonLayout[i].InstanceDataStepRate = 1;
	i++;

	polygonLayout[i].SemanticName = "TEXCOORD";
	polygonLayout[i].SemanticIndex = i - 1;
	polygonLayout[i].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	polygonLayout[i].InputSlot = 1;
	polygonLayout[i].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	polygonLayout[i].InputSlotClass = D3D11_INPUT_PER_INSTANCE_DATA;
	polygonLayout[i].InstanceDataStepRate = 1;
	i++;

	polygonLayout[i].SemanticName = "TEXCOORD";
	polygonLayout[i].SemanticIndex = i - 1;
	polygonLayout[i].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	polygonLayout[i].InputSlot = 1;
	polygonLayout[i].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	polygonLayout[i].InputSlotClass = D3D11_INPUT_PER_INSTANCE_DATA;
	polygonLayout[i].InstanceDataStepRate = 1;
	i++;

	polygonLayout[i].SemanticName = "TEXCOORD";
	polygonLayout[i].SemanticIndex = i - 1;
	polygonLayout[i].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	polygonLayout[i].InputSlot = 1;
	polygonLayout[i].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	polygonLayout[i].InputSlotClass = D3D11_INPUT_PER_INSTANCE_DATA;
	polygonLayout[i].InstanceDataStepRate = 1;
	i++;

	polygonLayout[i].SemanticName = "TEXCOORD";
	polygonLayout[i].SemanticIndex = i - 1;
	polygonLayout[i].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	polygonLayout[i].InputSlot = 1;
	polygonLayout[i].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	polygonLayout[i].InputSlotClass = D3D11_INPUT_PER_INSTANCE_DATA;
	polygonLayout[i].InstanceDataStepRate = 1;
	i++;

	unsigned int numElements = sizeof(polygonLayout) / sizeof(polygonLayout[0]);

	// Create the vertex input layout.
	HRESULT result = DX11::Device->CreateInputLayout(polygonLayout, numElements, aVS.data(), aVS.size(), myLayout.ReleaseAndGetAddressOf());
	if (FAILED(result))
	{
		ERROR_PRINT("%s", "Layout error");
	}
	return true;
}


