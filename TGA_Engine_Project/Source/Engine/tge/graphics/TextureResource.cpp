#include "stdafx.h"
#include "TextureResource.h"

#include <tge/graphics/DX11.h>

using namespace Tga;

TextureResource::TextureResource() : mySRV(nullptr) {}
TextureResource::TextureResource(ID3D11ShaderResourceView* aSRV) : mySRV(aSRV) {}
TextureResource::~TextureResource() {}

void TextureResource::SetAsResourceOnSlot(unsigned int aSlot) const
{
	assert(mySRV.Get());

	DX11::Context->PSSetShaderResources(aSlot, 1, mySRV.GetAddressOf());
}

void TextureResource::SetShaderResourceView(ID3D11ShaderResourceView* aSRV) 
{ 
	mySRV = ComPtr<ID3D11ShaderResourceView>(aSRV); 
}

Vector2ui TextureResource::CalculateTextureSize() const
{
	ID3D11Resource* resource = nullptr;
	mySRV->GetResource(&resource);
	if (!resource)
	{
		return Vector2ui(0, 0);
	}
	ID3D11Texture2D* txt = reinterpret_cast<ID3D11Texture2D*>(resource);
	D3D11_TEXTURE2D_DESC desc;
	txt->GetDesc(&desc);

	Vector2ui size(desc.Width, desc.Height);
	resource->Release();

	return size;
}