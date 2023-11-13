#include "stdafx.h"

#include <tge/shaders/shader.h>
#include <tge/engine.h>
#include <tge/graphics/GraphicsEngine.h>
#include <tge/graphics/DX11.h>
#include <tge/light/LightManager.h>
#include <tge/shaders/ShaderCommon.h>
#include <tge/filewatcher/FileWatcher.h>
#include <tge/texture/TextureManager.h>
#include <tge/EngineDefines.h>
#include <tge/util/StringCast.h>

Tga::Shader::Shader(Engine& aEngine)
	:myEngine(&aEngine)
	, myDirect3dEngine(&aEngine.GetGraphicsEngine())
	, myRandomSeed(rand() % 100)
{
	myIsReadyToRender = false;
}

Tga::Shader::~Shader()
{}

bool Tga::Shader::CreateShaders(const wchar_t* aVertex, const wchar_t* aPixel, callback_layout aLayout)
{
	myIsReadyToRender = false;
	myVertexShaderFile = aVertex;
	myPixelShaderFile = aPixel;

	std::string vsData;
	if (!DX11::LoadVertexShader(string_cast<std::string>(aVertex).c_str(), myVertexShader.ReleaseAndGetAddressOf(), vsData))
		return false;

	if (!DX11::LoadPixelShader(string_cast<std::string>(aPixel).c_str(), myPixelShader.ReleaseAndGetAddressOf()))
		return false;

	if (myLayout)
	{
		myLayout.Reset();
	}

	if (aLayout)
	{
		aLayout(vsData);
	}
	else if (!CreateInputLayout(vsData))
	{
		// LAYOUT
		D3D11_INPUT_ELEMENT_DESC polygonLayout[7];

		polygonLayout[0].SemanticName = "POSITION";
		polygonLayout[0].SemanticIndex = 0;
		polygonLayout[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
		polygonLayout[0].InputSlot = 0;
		polygonLayout[0].AlignedByteOffset = 0;
		polygonLayout[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
		polygonLayout[0].InstanceDataStepRate = 0;

		polygonLayout[1].SemanticName = "TEXCOORD";
		polygonLayout[1].SemanticIndex = 0;
		polygonLayout[1].Format = DXGI_FORMAT_R32G32_FLOAT;
		polygonLayout[1].InputSlot = 0;
		polygonLayout[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
		polygonLayout[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
		polygonLayout[1].InstanceDataStepRate = 0;

		polygonLayout[2].SemanticName = "TEXCOORD";
		polygonLayout[2].SemanticIndex = 1;
		polygonLayout[2].Format = DXGI_FORMAT_R32G32_FLOAT;
		polygonLayout[2].InputSlot = 1;
		polygonLayout[2].AlignedByteOffset = 0;
		polygonLayout[2].InputSlotClass = D3D11_INPUT_PER_INSTANCE_DATA;
		polygonLayout[2].InstanceDataStepRate = 1;

		polygonLayout[3].SemanticName = "TEXCOORD";
		polygonLayout[3].SemanticIndex = 2;
		polygonLayout[3].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
		polygonLayout[3].InputSlot = 1;
		polygonLayout[3].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
		polygonLayout[3].InputSlotClass = D3D11_INPUT_PER_INSTANCE_DATA;
		polygonLayout[3].InstanceDataStepRate = 1;

		polygonLayout[4].SemanticName = "TEXCOORD";
		polygonLayout[4].SemanticIndex = 3;
		polygonLayout[4].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
		polygonLayout[4].InputSlot = 1;
		polygonLayout[4].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
		polygonLayout[4].InputSlotClass = D3D11_INPUT_PER_INSTANCE_DATA;
		polygonLayout[4].InstanceDataStepRate = 1;

		polygonLayout[5].SemanticName = "TEXCOORD";
		polygonLayout[5].SemanticIndex = 4;
		polygonLayout[5].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
		polygonLayout[5].InputSlot = 1;
		polygonLayout[5].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
		polygonLayout[5].InputSlotClass = D3D11_INPUT_PER_INSTANCE_DATA;
		polygonLayout[5].InstanceDataStepRate = 1;

		polygonLayout[6].SemanticName = "TEXCOORD";
		polygonLayout[6].SemanticIndex = 5;
		polygonLayout[6].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
		polygonLayout[6].InputSlot = 1;
		polygonLayout[6].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
		polygonLayout[6].InputSlotClass = D3D11_INPUT_PER_INSTANCE_DATA;
		polygonLayout[6].InstanceDataStepRate = 1;


		unsigned int numElements = sizeof(polygonLayout) / sizeof(polygonLayout[0]);

		// Create the vertex input layout.
		HRESULT result = DX11::Device->CreateInputLayout(polygonLayout, numElements, vsData.data(), vsData.size(), myLayout.ReleaseAndGetAddressOf());
		if (FAILED(result))
		{
			ERROR_PRINT("%s", "Layout error");
			return false;
		}
	}

	myIsReadyToRender = true;
	return true;
}

bool Tga::Shader::PrepareRender(const RenderObjectSharedData& aSharedData) const
{
	if (!myVertexShader || !myPixelShader || !myIsReadyToRender || !myEngine)
	{
		return false;
	}

	DX11::RenderStateManager->SetSamplerState(aSharedData.mySamplerFilter, aSharedData.mySamplerAddressMode);
	DX11::RenderStateManager->SetBlendState(aSharedData.myBlendState);
	DX11::Context->VSSetShader(myVertexShader.Get(), NULL, 0);
	DX11::Context->PSSetShader(myPixelShader.Get(), NULL, 0);
	DX11::Context->IASetInputLayout(myLayout.Get());
	DX11::Context->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	DoOneFrameUpdates();

	return true;
}

void Tga::Shader::DoOneFrameUpdates() const
{
	myEngine->GetGraphicsEngine().UpdateAndBindCommonBuffer();
	myEngine->GetGraphicsEngine().UpdateAndBindLightBuffer();
}