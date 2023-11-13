#include "stdafx.h"
#include "AmbientLight.h"

#include <tge/graphics/DX11.h>
#include <DDSTextureLoader/DDSTextureLoader11.h>

using namespace Tga;

AmbientLight::AmbientLight(const Color& aColor, float aIntensity) : Light(Transform{}, aColor, aIntensity) {}
AmbientLight::AmbientLight(std::wstring& someCubemapLocation, const Color& aColor, float aIntensity) : Light(Transform{}, aColor, aIntensity) { SetCubemap(someCubemapLocation); }
AmbientLight::AmbientLight(const AmbientLight&) = default;
AmbientLight::~AmbientLight(){}
AmbientLight& AmbientLight::operator=(const AmbientLight& other) = default;

void AmbientLight::SetCubemap(std::wstring& someCubemapLocation)
{
    HRESULT result = DirectX::CreateDDSTextureFromFile(DX11::Device, someCubemapLocation.c_str(), nullptr, &myCubemap);
    assert(!FAILED(result));

	ID3D11Resource* theResource;
	myCubemap->GetResource(&theResource);
	ID3D11Texture2D* cubeTexture = reinterpret_cast<ID3D11Texture2D*>(theResource);
	D3D11_TEXTURE2D_DESC cubeDescription;
	cubeTexture->GetDesc(&cubeDescription);

	myNumMips = cubeDescription.MipLevels;
}
