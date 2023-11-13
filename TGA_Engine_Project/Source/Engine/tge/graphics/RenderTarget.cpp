#include "stdafx.h"
#include "RenderTarget.h"
#include <tge/Graphics/DepthBuffer.h>
#include <tge/graphics/DX11.h>

using namespace Tga;

RenderTarget::RenderTarget() : myRenderTarget(nullptr), myViewport(nullptr) {}

RenderTarget::~RenderTarget() {}

RenderTarget RenderTarget::Create(Vector2ui aSize, DXGI_FORMAT aFormat)
{
	HRESULT result;
	D3D11_TEXTURE2D_DESC desc = { 0 };
	desc.Width = aSize.X;
	desc.Height = aSize.Y;
	desc.MipLevels = 1;
	desc.ArraySize = 1;
	desc.Format = aFormat;
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	desc.CPUAccessFlags = 0;
	desc.MiscFlags = 0;

	ID3D11Texture2D* texture;
	result = DX11::Device->CreateTexture2D(&desc, nullptr, &texture);
	assert(SUCCEEDED(result));

	RenderTarget textureResult = Create(texture);

	ID3D11ShaderResourceView* SRV;
	result = DX11::Device->CreateShaderResourceView(texture, nullptr, &SRV);
	assert(SUCCEEDED(result));
	textureResult.mySRV = SRV;
	SRV->Release();

	return textureResult;
}

RenderTarget RenderTarget::Create(ID3D11Texture2D* aTexture)
{
	HRESULT result;

	ID3D11RenderTargetView* RTV;
	result = DX11::Device->CreateRenderTargetView(
		aTexture,
		nullptr,
		&RTV);
	assert(SUCCEEDED(result));

	RenderTarget textureResult;

	if (aTexture)
	{
		D3D11_TEXTURE2D_DESC desc;
		aTexture->GetDesc(&desc);
		textureResult.myViewport = std::make_shared<const D3D11_VIEWPORT>(D3D11_VIEWPORT{
				0,
				0,
				static_cast<float>(desc.Width),
				static_cast<float>(desc.Height),
				0,
				1
			});
	}
	
	textureResult.myRenderTarget = RTV;
	RTV->Release();
	return textureResult;
}

void RenderTarget::Clear(Vector4f aClearColor)
{
	DX11::Context->ClearRenderTargetView(myRenderTarget.Get(), &aClearColor.X);
}

void RenderTarget::SetAsActiveTarget(DepthBuffer* aDepth)
{
	if(aDepth)
	{
		DX11::Context->OMSetRenderTargets(1, myRenderTarget.GetAddressOf(), aDepth->GetDepthStencilView());
	}
	else
	{
		DX11::Context->OMSetRenderTargets(1, myRenderTarget.GetAddressOf(), nullptr);
	}

	DX11::Context->RSSetViewports(1, myViewport.get());
}
