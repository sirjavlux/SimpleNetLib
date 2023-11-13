#include "stdafx.h"
#include "FullscreenPixelateEffect.h"

#include "DX11.h"
#include "GraphicsEngine.h"

using namespace Tga;
FullscreenPixelateEffect::~FullscreenPixelateEffect()
{
	delete myViewport;
}

void FullscreenPixelateEffect::SetPixelSize(float aPixelSize)
{
	myPixelSize = aPixelSize;
}

bool FullscreenPixelateEffect::Init(const char* aPixelShaderPath)
{	
	D3D11_BUFFER_DESC bufferDescription = {};
	bufferDescription.Usage = D3D11_USAGE_DYNAMIC;
	bufferDescription.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bufferDescription.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	bufferDescription.ByteWidth = sizeof(EffectBufferData);
	HRESULT hresult = DX11::Device->CreateBuffer(&bufferDescription, nullptr, myEffectBuffer.GetAddressOf());

	if(FAILED(hresult))
		return false;

	ID3D11Texture2D* backBufferTexture;
	hresult = DX11::SwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&backBufferTexture);

	D3D11_TEXTURE2D_DESC backBufferDesc = {};
	backBufferTexture->GetDesc(&backBufferDesc);
	backBufferTexture->Release();

	D3D11_TEXTURE2D_DESC desc = {};
	desc.Width = backBufferDesc.Width;
	desc.Height = backBufferDesc.Height;
	desc.MipLevels = 1;
	desc.ArraySize = 1;
	desc.Format = backBufferDesc.Format;
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
	desc.CPUAccessFlags = 0;
	desc.MiscFlags = 0;

	hresult = DX11::Device->CreateTexture2D(&desc, nullptr, &myEffectTexture);
	if(FAILED(hresult))
		return false;

	hresult = DX11::Device->CreateShaderResourceView(myEffectTexture.Get(), nullptr, myEffectSRV.GetAddressOf());
	if(FAILED(hresult))
		return false;

	hresult = DX11::Device->CreateRenderTargetView(myEffectTexture.Get(), nullptr, myEffectRTV.GetAddressOf());
	if(FAILED(hresult))
		return false;

	myViewport = new D3D11_VIEWPORT(
	{
		0,
		0,
		static_cast<float>(backBufferDesc.Width),
		static_cast<float>(backBufferDesc.Height),
		0,
		1
	});

	return FullscreenEffect::Init(aPixelShaderPath);
}

void FullscreenPixelateEffect::Render()
{
	// We should not be the RTV here or we won't get any effects.
	// For now we can just force backbuffer :P Easier and we don't
	// allow depth testing here.
	DX11::BackBuffer->SetAsActiveTarget();

	myEffectBufferData.PixelSize = myPixelSize;
	myEffectBufferData.Resolution = Engine::GetInstance()->GetRenderSize();

	D3D11_MAPPED_SUBRESOURCE mappedResource;
	ZeroMemory(&mappedResource, sizeof(D3D11_MAPPED_SUBRESOURCE));

	const HRESULT hresult = DX11::Context->Map(myEffectBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if(FAILED(hresult))
		return;

	memcpy_s(mappedResource.pData, sizeof(EffectBufferData), &myEffectBufferData, sizeof(EffectBufferData));
	DX11::Context->Unmap(myEffectBuffer.Get(), 0);

	DX11::Context->PSSetConstantBuffers(13, 1, myEffectBuffer.GetAddressOf());
	DX11::Context->PSSetShaderResources(0, 1, myEffectSRV.GetAddressOf());

	FullscreenEffect::Render();
}

void FullscreenPixelateEffect::Activate(DepthBuffer* aDepth)
{
	const Color clearColor = Engine::GetInstance()->GetClearColor();
	DX11::Context->ClearRenderTargetView(myEffectRTV.Get(), &clearColor.myR);

	if(aDepth)
	{
		DX11::Context->OMSetRenderTargets(1, myEffectRTV.GetAddressOf(), aDepth->GetDepthStencilView());
	}
	else
	{
		DX11::Context->OMSetRenderTargets(1, myEffectRTV.GetAddressOf(), nullptr);
	}

	DX11::Context->RSSetViewports(1, myViewport);
}
