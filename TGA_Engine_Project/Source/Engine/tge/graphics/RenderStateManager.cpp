#include "stdafx.h"
#include "RenderStateManager.h"

#include <tge/graphics/DX11.h>

using namespace Tga;

bool RenderStateManager::Init()
{
	if (!CreateBlendStates())
		return false;
	if (!CreateDepthStencilStates())
		return false;
	if (!CreateRasterizerStates())
		return false;
	if (!CreateSamplers())
		return false;

	return true;
}

void RenderStateManager::SetBlendState(BlendState aBlendState)
{
	DX11::Context->OMSetBlendState(myBlendStates[(int)aBlendState].Get(), nullptr, 0xffffffff);
}

void RenderStateManager::SetDepthStencilState(DepthStencilState aDepthStencilState)
{
	DX11::Context->OMSetDepthStencilState(myDepthStencilStates[(int)aDepthStencilState].Get(), 0);
}

void RenderStateManager::SetRasterizerState(RasterizerState aRasterizerState)
{
	DX11::Context->RSSetState(myRasterizerStates[(int)aRasterizerState].Get());
}

void RenderStateManager::SetSamplerState(SamplerFilter aFilter, SamplerAddressMode aAddressMode)
{
	DX11::Context->PSSetSamplers(0, 1 + ShaderMap::MAP_MAX, mySamplerStates[(int)aFilter][(int)aAddressMode].GetAddressOf());
}

void RenderStateManager::SetAllStates(const RenderState& aRenderState)
{
	SetBlendState(aRenderState.blendState);
	SetDepthStencilState(aRenderState.depthStencilState);
	SetRasterizerState(aRenderState.rasterizerState);
	SetSamplerState(aRenderState.samplerFilter, aRenderState.samplerAddressMode);
}

void RenderStateManager::ResetStates()
{
	RenderState renderState;
	renderState.blendState = BlendState::Disabled;
	renderState.rasterizerState = RasterizerState::NoFaceCulling;
	renderState.depthStencilState = DepthStencilState::Write;
	renderState.samplerAddressMode = SamplerAddressMode::Wrap;
	renderState.samplerFilter = SamplerFilter::Trilinear;
	SetAllStates(renderState);
}

bool RenderStateManager::CreateBlendStates()
{
	HRESULT result = S_OK;
	D3D11_BLEND_DESC blendStateDescription = {};

	blendStateDescription.RenderTarget[0].BlendEnable = FALSE;
	blendStateDescription.RenderTarget[0].SrcBlend = D3D11_BLEND_ZERO;
	blendStateDescription.RenderTarget[0].DestBlend = D3D11_BLEND_ZERO;
	blendStateDescription.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	blendStateDescription.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ZERO;
	blendStateDescription.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	blendStateDescription.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	blendStateDescription.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	result = DX11::Device->CreateBlendState(&blendStateDescription, myBlendStates[(int)BlendState::Disabled].ReleaseAndGetAddressOf());
	if (FAILED(result))
	{
		return false;
	}

	blendStateDescription = {};
	blendStateDescription.RenderTarget[0].BlendEnable = TRUE;
	blendStateDescription.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	blendStateDescription.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	blendStateDescription.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	blendStateDescription.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	blendStateDescription.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ONE;
	blendStateDescription.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_MAX;
	blendStateDescription.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	result = DX11::Device->CreateBlendState(&blendStateDescription, myBlendStates[(int)BlendState::AlphaBlend].ReleaseAndGetAddressOf());
	if (FAILED(result))
	{
		return false;
	}

	blendStateDescription = {};
	blendStateDescription.RenderTarget[0].BlendEnable = TRUE;
	blendStateDescription.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	blendStateDescription.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;
	blendStateDescription.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	blendStateDescription.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	blendStateDescription.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ONE;
	blendStateDescription.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_MAX;
	blendStateDescription.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	result = DX11::Device->CreateBlendState(&blendStateDescription, myBlendStates[(int)BlendState::AdditiveBlend].ReleaseAndGetAddressOf());
	if (FAILED(result))
	{
		return false;
	}

	return true;
}

bool RenderStateManager::CreateDepthStencilStates()
{
	HRESULT result = S_OK;

	D3D11_DEPTH_STENCIL_DESC readOnlyDepthDesc = {};
	readOnlyDepthDesc.DepthEnable = true;
	readOnlyDepthDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
	readOnlyDepthDesc.DepthFunc = D3D11_COMPARISON_LESS;
	readOnlyDepthDesc.StencilEnable = false;

	result = DX11::Device->CreateDepthStencilState(&readOnlyDepthDesc, &myDepthStencilStates[(int)DepthStencilState::ReadOnly]);
	if (FAILED(result))
		return false;


	myDepthStencilStates[(int)DepthStencilState::Write] = nullptr;

	return true;
}

bool RenderStateManager::CreateRasterizerStates()
{
	HRESULT result = S_OK;

	D3D11_RASTERIZER_DESC rasterizerDesc = {};
	rasterizerDesc.FillMode = D3D11_FILL_WIREFRAME;
	rasterizerDesc.CullMode = D3D11_CULL_BACK;
	rasterizerDesc.DepthClipEnable = true;

	result = DX11::Device->CreateRasterizerState(&rasterizerDesc, &myRasterizerStates[(int)RasterizerState::Wireframe]);
	if (FAILED(result))
		return false;

	rasterizerDesc.CullMode = D3D11_CULL_NONE;

	result = DX11::Device->CreateRasterizerState(&rasterizerDesc, &myRasterizerStates[(int)RasterizerState::WireframeNoCulling]);
	if (FAILED(result))
		return false;

	rasterizerDesc = {};
	rasterizerDesc.AntialiasedLineEnable = false;
	rasterizerDesc.CullMode = D3D11_CULL_NONE;
	rasterizerDesc.DepthBias = 0;
	rasterizerDesc.DepthBiasClamp = 0.0f;
	rasterizerDesc.DepthClipEnable = true;
	rasterizerDesc.FillMode = D3D11_FILL_SOLID;
	rasterizerDesc.FrontCounterClockwise = false;
	rasterizerDesc.MultisampleEnable = true;
	rasterizerDesc.ScissorEnable = false;
	rasterizerDesc.SlopeScaledDepthBias = 0.0f;

	result = DX11::Device->CreateRasterizerState(&rasterizerDesc, &myRasterizerStates[(int)RasterizerState::NoFaceCulling]);
	if (FAILED(result))
		return false;

	myRasterizerStates[(int)RasterizerState::BackfaceCulling] = nullptr;

	return true;
}

bool RenderStateManager::CreateSamplers()
{
	HRESULT result = S_OK;
	D3D11_SAMPLER_DESC samplerDesc = {};

	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.MipLODBias = 0.0f;
	samplerDesc.MaxAnisotropy = 16;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samplerDesc.BorderColor[0] = 0;
	samplerDesc.BorderColor[1] = 0;
	samplerDesc.BorderColor[2] = 0;
	samplerDesc.BorderColor[3] = 0;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
	result = DX11::Device->CreateSamplerState(&samplerDesc, mySamplerStates[(int)SamplerFilter::Point][(int)SamplerAddressMode::Clamp].ReleaseAndGetAddressOf());
	if (FAILED(result))
	{
		return false;
	}

	samplerDesc = {};
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.MipLODBias = 0.0f;
	samplerDesc.MaxAnisotropy = 16;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samplerDesc.BorderColor[0] = 0;
	samplerDesc.BorderColor[1] = 0;
	samplerDesc.BorderColor[2] = 0;
	samplerDesc.BorderColor[3] = 0;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
	result = DX11::Device->CreateSamplerState(&samplerDesc, mySamplerStates[(int)SamplerFilter::Point][(int)SamplerAddressMode::Wrap].ReleaseAndGetAddressOf());
	if (FAILED(result))
	{
		return false;
	}

	samplerDesc = {};
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_MIRROR;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_MIRROR;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_MIRROR;
	samplerDesc.MipLODBias = 0.0f;
	samplerDesc.MaxAnisotropy = 16;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samplerDesc.BorderColor[0] = 0;
	samplerDesc.BorderColor[1] = 0;
	samplerDesc.BorderColor[2] = 0;
	samplerDesc.BorderColor[3] = 0;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
	result = DX11::Device->CreateSamplerState(&samplerDesc, mySamplerStates[(int)SamplerFilter::Point][(int)SamplerAddressMode::Mirror].ReleaseAndGetAddressOf());
	if (FAILED(result))
	{
		return false;
	}

	samplerDesc = {};
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_LINEAR_MIP_POINT;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.MipLODBias = 0.0f;
	samplerDesc.MaxAnisotropy = 16;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samplerDesc.BorderColor[0] = 0;
	samplerDesc.BorderColor[1] = 0;
	samplerDesc.BorderColor[2] = 0;
	samplerDesc.BorderColor[3] = 0;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
	result = DX11::Device->CreateSamplerState(&samplerDesc, mySamplerStates[(int)SamplerFilter::Bilinear][(int)SamplerAddressMode::Clamp].ReleaseAndGetAddressOf());
	if (FAILED(result))
	{
		return false;
	}

	samplerDesc = {};
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_LINEAR_MIP_POINT;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.MipLODBias = 0.0f;
	samplerDesc.MaxAnisotropy = 16;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samplerDesc.BorderColor[0] = 0;
	samplerDesc.BorderColor[1] = 0;
	samplerDesc.BorderColor[2] = 0;
	samplerDesc.BorderColor[3] = 0;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
	result = DX11::Device->CreateSamplerState(&samplerDesc, mySamplerStates[(int)SamplerFilter::Bilinear][(int)SamplerAddressMode::Wrap].ReleaseAndGetAddressOf());
	if (FAILED(result))
	{
		return false;
	}

	samplerDesc = {};
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_LINEAR_MIP_POINT;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_MIRROR;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_MIRROR;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_MIRROR;
	samplerDesc.MipLODBias = 0.0f;
	samplerDesc.MaxAnisotropy = 16;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samplerDesc.BorderColor[0] = 0;
	samplerDesc.BorderColor[1] = 0;
	samplerDesc.BorderColor[2] = 0;
	samplerDesc.BorderColor[3] = 0;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
	result = DX11::Device->CreateSamplerState(&samplerDesc, mySamplerStates[(int)SamplerFilter::Bilinear][(int)SamplerAddressMode::Mirror].ReleaseAndGetAddressOf());
	if (FAILED(result))
	{
		return false;
	}

	samplerDesc = {};
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.MipLODBias = 0.0f;
	samplerDesc.MaxAnisotropy = 16;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samplerDesc.BorderColor[0] = 0;
	samplerDesc.BorderColor[1] = 0;
	samplerDesc.BorderColor[2] = 0;
	samplerDesc.BorderColor[3] = 0;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
	result = DX11::Device->CreateSamplerState(&samplerDesc, mySamplerStates[(int)SamplerFilter::Trilinear][(int)SamplerAddressMode::Clamp].ReleaseAndGetAddressOf());
	if (FAILED(result))
	{
		return false;
	}

	samplerDesc = {};
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.MipLODBias = 0.0f;
	samplerDesc.MaxAnisotropy = 16;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samplerDesc.BorderColor[0] = 0;
	samplerDesc.BorderColor[1] = 0;
	samplerDesc.BorderColor[2] = 0;
	samplerDesc.BorderColor[3] = 0;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
	result = DX11::Device->CreateSamplerState(&samplerDesc, mySamplerStates[(int)SamplerFilter::Trilinear][(int)SamplerAddressMode::Wrap].ReleaseAndGetAddressOf());
	if (FAILED(result))
	{
		return false;
	}

	samplerDesc = {};
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_MIRROR;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_MIRROR;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_MIRROR;
	samplerDesc.MipLODBias = 0.0f;
	samplerDesc.MaxAnisotropy = 16;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samplerDesc.BorderColor[0] = 0;
	samplerDesc.BorderColor[1] = 0;
	samplerDesc.BorderColor[2] = 0;
	samplerDesc.BorderColor[3] = 0;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
	result = DX11::Device->CreateSamplerState(&samplerDesc, mySamplerStates[(int)SamplerFilter::Trilinear][(int)SamplerAddressMode::Mirror].ReleaseAndGetAddressOf());
	if (FAILED(result))
	{
		return false;
	}

	return true;
}