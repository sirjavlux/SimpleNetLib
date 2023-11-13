#include "stdafx.h"
#include "DepthBuffer.h"

#include <tge/graphics/DX11.h>

using namespace Tga;

void DepthBuffer::SetAsActiveTarget()
{
	DX11::Context->OMSetRenderTargets(0, nullptr, GetDepthStencilView());
	DX11::Context->RSSetViewports(1, &myViewport);
}

void DepthBuffer::Clear(float aClearDepthValue /* = 1.0f */, uint8_t aClearStencilValue /* = 0 */)
{
	DX11::Context->ClearDepthStencilView(myDepth.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, aClearDepthValue, aClearStencilValue);
}

DepthBuffer DepthBuffer::Create(Vector2ui aSize)
{
	HRESULT result;
	D3D11_TEXTURE2D_DESC desc = { 0 };
	desc.Width = aSize.X;
	desc.Height = aSize.Y;
	desc.MipLevels = 1;
	desc.ArraySize = 1;
	desc.Format = DXGI_FORMAT_R32_TYPELESS;
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
	desc.CPUAccessFlags = 0;
	desc.MiscFlags = 0;

	ID3D11Texture2D* texture;
	result = DX11::Device->CreateTexture2D(&desc, nullptr, &texture);
	assert(SUCCEEDED(result));

	ID3D11DepthStencilView* DSV;
	D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc{};

	dsvDesc.Flags = 0;
	dsvDesc.Format = DXGI_FORMAT_D32_FLOAT;
	dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	result = DX11::Device->CreateDepthStencilView(texture, &dsvDesc, &DSV);
	assert(SUCCEEDED(result));

	DepthBuffer textureResult;
	textureResult.myDepth = DSV;
	DSV->Release();

	ID3D11ShaderResourceView* SRV;
	D3D11_SHADER_RESOURCE_VIEW_DESC srDesc{};
	srDesc.Format = DXGI_FORMAT_R32_FLOAT;
	srDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srDesc.Texture2D.MostDetailedMip = 0;
	srDesc.Texture2D.MipLevels = std::numeric_limits<UINT>::max();
	result = DX11::Device->CreateShaderResourceView(texture, &srDesc, &SRV);
	assert(SUCCEEDED(result));
	textureResult.mySRV = SRV;
	SRV->Release();

	textureResult.myViewport = {
			0,
			0,
			static_cast<float>(aSize.X),
			static_cast<float>(aSize.Y),
			0,
			1
	};
	return textureResult;
}
