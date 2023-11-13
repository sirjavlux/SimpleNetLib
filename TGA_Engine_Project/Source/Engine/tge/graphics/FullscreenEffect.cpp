#include "stdafx.h"
#include "FullscreenEffect.h"

#include <fstream>

#include <tge/graphics/DX11.h>

using namespace Tga;

FullscreenEffect::FullscreenEffect()
{

}

bool FullscreenEffect::Init(const char* aPixelShaderPath)
{
	std::string vsData;
	if (!DX11::LoadVertexShader("Shaders/PostprocessVS.cso", &myVertexShader, vsData))
		return false;

	if (!DX11::LoadPixelShader(aPixelShaderPath, &myPixelShader))
		return false;

	return true;
}

void FullscreenEffect::Render()
{
	DX11::Context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY::D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	DX11::Context->IASetInputLayout(nullptr);
	DX11::Context->IASetVertexBuffers(0, 0, nullptr, nullptr, nullptr);
	DX11::Context->IASetIndexBuffer(nullptr, DXGI_FORMAT_UNKNOWN, 0);

	DX11::Context->VSSetShader(myVertexShader.Get(), nullptr, 0);
	DX11::Context->GSSetShader(nullptr, nullptr, 0);
	DX11::Context->PSSetShader(myPixelShader.Get(), nullptr, 0);
	DX11::LogDrawCall();
	DX11::Context->Draw(3, 0);
}