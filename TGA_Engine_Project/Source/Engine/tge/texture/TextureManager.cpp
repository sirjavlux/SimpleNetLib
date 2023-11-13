#include "stdafx.h"

#include <tge/texture/TextureManager.h>
#include <DDSTextureLoader/DDSTextureLoader11.h>
#include <WICTextureLoader/WICTextureLoader11.h>
#include <tge/engine.h>
#include <tge/graphics/DX11.h>
#include <tge/noise/PerlinNoise.h>
#include <tge/Math/color.h>
#include <d3d11.h>
#include <tge/EngineDefines.h>
#include <tge/filewatcher/FileWatcher.h>
#include "xxh64_en.hpp"
#include <tge/settings/settings.h>

//  Define min max macros required by GDI+ headers.
#ifndef max
#define max(a,b) (((a) > (b)) ? (a) : (b))
#else
#error max macro is already defined
#endif
#ifndef min
#define min(a,b) (((a) < (b)) ? (a) : (b))
#else
#error min macro is already defined
#endif

#pragma warning( disable : 4458 )
#include <gdiplus.h>

//  Undefine min max macros so they won't collide with <limits> header content.
#undef min
#undef max


using namespace Tga;


TextureManager::TextureManager(void)
	: myFailedResource(nullptr)
	, myDefaultNormalMapResource(nullptr)
{
}

TextureManager::~TextureManager(void)
{}

void SetDebugObjectName(_In_ ID3D11DeviceChild* resource, _In_z_ std::wstring aName)
{
#if defined(_DEBUG)
	resource->SetPrivateData(WKPDID_D3DDebugObjectName, static_cast<UINT>(aName.size()), aName.c_str());
#else
	resource;
	aName;
#endif
}

bool IsDDS(std::wstring aPath)
{
	if (aPath.substr(aPath.find_last_of(L".") + 1) == L"dds")
	{
		return true;
	}
	else 
	{
		return false;
	}
}

bool IsTarga(std::wstring aPath)
{
	if (aPath.substr(aPath.find_last_of(L".") + 1) == L"tga")
	{
		return true;
	}
	else
	{
		return false;
	}
}

Texture* TextureManager::GetTexture(const wchar_t* aTexturePath, bool aForceSRGB, bool aForceReload)
{
	if (!aTexturePath)
	{
		aTexturePath = L"";
	}
	
	std::wstring toString = aTexturePath;
	const uint64_t hashedID = xxh64::hash((char*)toString.c_str(), sizeof(wchar_t)*toString.length(), 0);
	auto it = std::find_if(myResourceViews.begin(), myResourceViews.end(), [hashedID](const std::unique_ptr<Texture>& s) { return s->myID == hashedID; });
	
	Texture* loadedTexture = nullptr;
	if (it != myResourceViews.end())
	{
		loadedTexture = it->get();
	}
	if (!aForceReload && loadedTexture)
	{
		Texture* texture = it->get();
		return texture;
	}

	ComPtr<ID3D11ShaderResourceView> resource;
	const std::wstring asset_path = Settings::ResolveAssetPathW(aTexturePath);

	HRESULT hr = DirectX::CreateDDSTextureFromFileEx(DX11::Device, nullptr,
		asset_path.c_str(), 0,
		D3D11_USAGE_DEFAULT, D3D11_BIND_SHADER_RESOURCE, 0, 0,
		aForceSRGB,
		nullptr, resource.ReleaseAndGetAddressOf(), nullptr);

	if (FAILED(hr))
	{
		if (!CAN_USE_OTHER_FORMATS_THAN_DDS)
		{
			ERROR_PRINT("%s %s", "This image format is forbidden! Use .dds! ", asset_path.c_str());
			hr = E_FAIL;
		}
		else if (DX11::IsOnSameThreadAsEngine())
		{
			hr = DirectX::CreateWICTextureFromFileEx(
				DX11::Device, 
				DX11::Context, 
				asset_path.c_str(),
				16384,
				D3D11_USAGE_DEFAULT,
				D3D11_BIND_SHADER_RESOURCE,
				0,
				0,
				aForceSRGB ? DirectX::WIC_LOADER_FORCE_SRGB : DirectX::WIC_LOADER_DEFAULT,
				nullptr, 
				resource.ReleaseAndGetAddressOf());

			if (FAILED(hr))
			{
				if (IsTarga(asset_path))
				{
					Tga32 targa;
					Tga32::Image* image = targa.Load(asset_path.c_str());
					if(image)
					{
						loadedTexture = CreateTextureFromTarga(image);
						if (loadedTexture)
						{
							resource = loadedTexture->GetShaderResourceView();
							hr = S_OK;
						}
					}
					else
					{
						ERROR_PRINT("%s %s", "This targa image is not supported, please use 32bit non compressed ", asset_path.c_str());
					}
				}
				else
				{
					if (hr == 0x80070002) // file not found
					{
						ERROR_PRINT("%s %s", "Image file not found!", asset_path.c_str());
					}
					else
					{
						ERROR_PRINT("%s %s", "This image format is forbidden! Use .dds, png, tga! ", asset_path.c_str());
					}
				}
				
			}
		}
		else
		{
			INFO_PRINT("Trying to load a non-dds or a wierd format of dds on another thread than the engine. This is not supported, choose a correct dds format");
		}
		
	}

	if (!FAILED(hr))
	{
		Vector2f textureSize = GetTextureSize(resource.Get(), false);
		unsigned int x = static_cast<int>(textureSize.x);
		unsigned int y = static_cast<int>(textureSize.y);

		bool powerOfTwo = !(x == 0) && !(x & (x - 1));
		powerOfTwo &= !(y == 0) && !(y & (y - 1));
		if (!powerOfTwo && IsDDS(asset_path))
		{
			if (!CAN_USE_DDS_NOT_POWER_OF_TWO)
			{
				ERROR_PRINT("%s %s", "DDS is not power of two, this is forbidden! ", asset_path.c_str());
				hr = E_FAIL;
			}
			else
			{
				INFO_TIP("%s %s %i*%i", "DDS needs to be power of two!", asset_path.c_str(), x, y);
			}
		}

	}
	
	if (!loadedTexture)
	{
		Engine::GetInstance()->GetFileWatcher()->WatchFileChange(asset_path, std::bind(&Tga::TextureManager::OnTextureChanged, this, std::placeholders::_1));
	}
	if (FAILED(hr))
	{
		ERROR_PRINT("%s %s", "Failed to load resource: ", asset_path.c_str());
		resource = myFailedResource;

		Texture* newTexture = loadedTexture;
		if (!loadedTexture)
		{
			newTexture = new Texture();
		}
		newTexture->myPath = asset_path;
		newTexture->SetShaderResourceView(resource.Get());
		newTexture->myID = hashedID;
		SetDebugObjectName(newTexture->GetShaderResourceView(), asset_path);
		newTexture->myIsFailedTexture = true;

		Vector2f texSize = GetTextureSize(resource.Get());
		newTexture->mySize = Vector2f(0.3f, 0.3f);
		newTexture->myImageSize = Vector2f(512, 512);

		if (!loadedTexture)
		{
			myResourceViews.push_back(std::unique_ptr<Texture>(newTexture));
		}
		return newTexture;
	}

	if (resource)
	{
		Texture* newTexture = loadedTexture;
		if (!loadedTexture)
		{
			newTexture = new Texture();
		}
		newTexture->myPath = asset_path;
		newTexture->myID = hashedID;
		newTexture->SetShaderResourceView(resource.Get());
		SetDebugObjectName(newTexture->GetShaderResourceView(), asset_path);

		Vector2f texSize = GetTextureSize(resource.Get());
		newTexture->mySize = texSize;
		
		newTexture->myImageSize = GetTextureSize(resource.Get(), false);

		if (!loadedTexture)
		{
			myResourceViews.push_back(std::unique_ptr<Texture>(newTexture));
		}

		return newTexture;
	}

	return nullptr;
}

Texture* TextureManager::TryGetTexture(const wchar_t* aTexturePath, bool aForceSRGB)
{
	if (!aTexturePath)
	{
		return nullptr;
	}

	std::wstring toString = std::wstring(aTexturePath);
	const uint64_t hashedID = xxh64::hash((char*)toString.c_str(), sizeof(wchar_t) * toString.length(), 0);
	auto it = std::find_if(myResourceViews.begin(), myResourceViews.end(), [hashedID](const std::unique_ptr<Texture>& s) { return s->myID == hashedID; });

	Texture* loadedTexture = nullptr;
	if (it != myResourceViews.end())
	{
		loadedTexture = it->get();
	}
	if (loadedTexture)
	{
		Texture* texture = it->get();
		return texture;
	}

	ComPtr<ID3D11ShaderResourceView> resource;
	HRESULT hr = DirectX::CreateDDSTextureFromFileEx(DX11::Device, nullptr,
		aTexturePath, 0,
		D3D11_USAGE_DEFAULT, D3D11_BIND_SHADER_RESOURCE, 0, 0,
		aForceSRGB,
		nullptr, resource.ReleaseAndGetAddressOf(), nullptr);

	if (FAILED(hr))
	{
		if (!CAN_USE_OTHER_FORMATS_THAN_DDS)
		{
			ERROR_PRINT("%s %s", "This image format is forbidden! Use .dds! ", aTexturePath);
			hr = E_FAIL;
		}
		else if (DX11::IsOnSameThreadAsEngine())
		{
			hr = DirectX::CreateWICTextureFromFile(DX11::Device, DX11::Context, aTexturePath, nullptr,
				resource.ReleaseAndGetAddressOf());
			if (FAILED(hr))
			{
				if (IsTarga(aTexturePath))
				{
					Tga32 targa;
					Tga32::Image* image = targa.Load(aTexturePath);
					if (image)
					{
						loadedTexture = CreateTextureFromTarga(image);
						if (loadedTexture)
						{
							resource = loadedTexture->GetShaderResourceView();
							hr = S_OK;
						}
					}
					else
					{
						ERROR_PRINT("%s %s", "This targa image is not supported, please use 32bit non compressed ", aTexturePath);
					}
				}
				else
				{
					if (hr == 0x80070002) // file not found
					{
						return nullptr;
					}
					else
					{
						ERROR_PRINT("%s %s", "This image format is forbidden! Use .dds, png, tga! ", aTexturePath);
					}
				}

			}
		}
		else
		{
			INFO_PRINT("Trying to load a non-dds or a wierd format of dds on another thread than the engine. This is not supported, choose a correct dds format");
		}

	}

	if (!FAILED(hr))
	{
		Vector2f textureSize = GetTextureSize(resource.Get(), false);
		unsigned int x = static_cast<int>(textureSize.x);
		unsigned int y = static_cast<int>(textureSize.y);

		bool powerOfTwo = !(x == 0) && !(x & (x - 1));
		powerOfTwo &= !(y == 0) && !(y & (y - 1));
		if (!powerOfTwo && IsDDS(aTexturePath))
		{
			if (!CAN_USE_DDS_NOT_POWER_OF_TWO)
			{
				ERROR_PRINT("%s %s", "DDS is not power of two, this is forbidden! ", aTexturePath);
				hr = E_FAIL;
			}
			else
			{
				INFO_TIP("%s %s %i*%i", "DDS needs to be power of two!", aTexturePath, x, y);
			}
		}

	}

	if (resource)
	{
		Texture* newTexture = loadedTexture;
		if (!loadedTexture)
		{
			newTexture = new Texture();
		}
		newTexture->myPath = aTexturePath;
		newTexture->myID = hashedID;
		newTexture->SetShaderResourceView(resource.Get());
		SetDebugObjectName(newTexture->GetShaderResourceView(), aTexturePath);

		Vector2f texSize = GetTextureSize(resource.Get());
		newTexture->mySize = texSize;

		newTexture->myImageSize = GetTextureSize(resource.Get(), false);

		if (!loadedTexture)
		{
			myResourceViews.push_back(std::unique_ptr<Texture>(newTexture));
		}

		return newTexture;
	}

	return nullptr;
}

void TextureManager::OnTextureChanged(std::wstring aFile)
{
	std::wstring notWide = aFile;
	std::replace(notWide.begin(), notWide.end(), '\\', '/');
	for (int i=0; i< myResourceViews.size(); i++)
	{
		if (myResourceViews[i]->myPath.compare(notWide) != std::wstring::npos)
		{
			INFO_PRINT("%s%s", "Texture changed: ", notWide.c_str());
			GetTexture(notWide.c_str(), true);
			break;
		}
	}
}

DXGI_FORMAT TextureManager::GetTextureFormat(ID3D11ShaderResourceView* aResourceView) const
{
	ID3D11Resource* resource = nullptr;
	aResourceView->GetResource(&resource);
	if (!resource)
	{
		return DXGI_FORMAT_UNKNOWN;
	}
	ID3D11Texture2D* txt = reinterpret_cast<ID3D11Texture2D*>(resource);
	D3D11_TEXTURE2D_DESC desc;
	txt->GetDesc(&desc);
	resource->Release();
	return desc.Format;
}

Vector2f TextureManager::GetTextureSize( ID3D11ShaderResourceView* aResourceView, bool aNormalize)
{
	ID3D11Resource* resource = nullptr;
	aResourceView->GetResource(&resource);
	if (!resource)
	{
		return Vector2f(0, 0);
	}
	ID3D11Texture2D* txt = reinterpret_cast<ID3D11Texture2D*>( resource );
	D3D11_TEXTURE2D_DESC desc;
	txt->GetDesc( &desc );

	Vector2f size(static_cast<float>(desc.Width), static_cast<float>(desc.Height));
	resource->Release();

	Vector2f windowSize;
	windowSize.x = static_cast<float>(Engine::GetInstance()->GetTargetSize().y);
	windowSize.y = static_cast<float>(Engine::GetInstance()->GetTargetSize().y);

	if (aNormalize)
	{
		return size / windowSize;
	}
	return size;
}

Texture* TextureManager::CreateTextureFromTarga(Tga32::Image* aImage)
{
	if (!aImage)
	{
		return nullptr;
	}
	ID3D11Texture2D* texture = nullptr;
	Texture *tex = nullptr;
	ID3D11ShaderResourceView* resource = nullptr;

	D3D11_TEXTURE2D_DESC tdesc;
	D3D11_SUBRESOURCE_DATA tbsd;

	int h = aImage->myHeight;
	int w = aImage->myWidth;
	int *buf = new int[h*w];
	for (int i = 0; i < (h*w) * 4; i+=4)
	{
		unsigned char b = static_cast<unsigned char>(aImage->myImage[i]);
		unsigned char g = static_cast<unsigned char>(aImage->myImage[i+1]);
		unsigned char r = static_cast<unsigned char>(aImage->myImage[i+2]);
		unsigned char a = static_cast<unsigned char>(aImage->myImage[i+3]);

		unsigned int final = 0;
		final |= (a << 24);
		final |= (r << 16);
		final |= (g << 8);
		final |= (b);

		buf[i/4] = final;
	}

	tbsd.pSysMem = (void *)buf;
	tbsd.SysMemPitch = w * 4;
	tbsd.SysMemSlicePitch = w * h * 4;

	tdesc.Width = w;
	tdesc.Height = h;
	tdesc.MipLevels = 1;
	tdesc.ArraySize = 1;

	tdesc.SampleDesc.Count = 1;
	tdesc.SampleDesc.Quality = 0;
	tdesc.Usage = D3D11_USAGE_DYNAMIC;
	tdesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	tdesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	tdesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	tdesc.MiscFlags = 0;

	if (FAILED(DX11::Device->CreateTexture2D(&tdesc, &tbsd, &texture)))
		return nullptr;

	delete[] buf;

	HRESULT hr = DX11::Device->CreateShaderResourceView(texture, NULL, &resource);
	if (FAILED(hr))
	{
		return nullptr;
	}

	if(resource)
	{
		tex = new Texture();
		tex->myImageSize.Set(aImage->myWidth, aImage->myHeight);
		tex->myPath = L"Lol";
		tex->SetShaderResourceView(resource);
	}

	return tex;
}

void TextureManager::CreateErrorSquareTexture()
{
	HRESULT hr;

	ComPtr<ID3D11Texture2D> tex;
	D3D11_TEXTURE2D_DESC tdesc;
	D3D11_SUBRESOURCE_DATA tbsd;

	int h = 16;
	int w = 16;
	int *buf = new int[h*w];
	for(int i=0;i<h;i++)
	{
		for(int j=0;j<w;j++)
		{
			if ((i+j) % 2 == 0)
			{
				buf[i*w+j] = 0xff000000;
			}
			else
			{
				buf[i*w+j] = 0xffff00ff;
			}
		}
	}

	tbsd.pSysMem = (void *)buf;
	tbsd.SysMemPitch = w*4;
	tbsd.SysMemSlicePitch = w*h*4; // Not needed since this is a 2d texture

	tdesc.Width = w;
	tdesc.Height = h;
	tdesc.MipLevels = 1;
	tdesc.ArraySize = 1;

	tdesc.SampleDesc.Count = 1;
	tdesc.SampleDesc.Quality = 0;
	tdesc.Usage = D3D11_USAGE_DEFAULT;
	tdesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	tdesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;

	tdesc.CPUAccessFlags = 0;
	tdesc.MiscFlags = 0;

	if(FAILED( DX11::Device->CreateTexture2D(&tdesc,&tbsd,&tex)))
		return;

	delete[] buf;

	hr = DX11::Device->CreateShaderResourceView(tex.Get(), NULL, myFailedResource.ReleaseAndGetAddressOf());
	if (FAILED(hr))
	{
		return;
	}
}

ComPtr<ID3D11ShaderResourceView> TextureManager::CreateWhiteSquareTexture()
{
	HRESULT hr;

	ComPtr<ID3D11Texture2D> tex;
	D3D11_TEXTURE2D_DESC tdesc;
	D3D11_SUBRESOURCE_DATA tbsd;

	int h = 4;
	int w = 4;
	int *buf = new int[h*w];
	for (int i = 0; i < h; i++)
	{
		for (int j = 0; j < w; j++)
		{
			buf[i*w + j] = 0xffffffff;
		}
	}

	tbsd.pSysMem = (void *)buf;
	tbsd.SysMemPitch = w * 4;
	tbsd.SysMemSlicePitch = w*h * 4; // Not needed since this is a 2d texture

	tdesc.Width = w;
	tdesc.Height = h;
	tdesc.MipLevels = 1;
	tdesc.ArraySize = 1;

	tdesc.SampleDesc.Count = 1;
	tdesc.SampleDesc.Quality = 0;
	tdesc.Usage = D3D11_USAGE_DEFAULT;
	tdesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	tdesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;

	tdesc.CPUAccessFlags = 0;
	tdesc.MiscFlags = 0;

	if (FAILED(DX11::Device->CreateTexture2D(&tdesc, &tbsd, &tex)))
		return nullptr;

	delete[] buf;

	ComPtr<ID3D11ShaderResourceView> resource;
	hr = DX11::Device->CreateShaderResourceView(tex.Get(), NULL, resource.ReleaseAndGetAddressOf());
	if (FAILED(hr))
	{
		return nullptr;
	}
	return resource;
}

void TextureManager::CreateDefaultNormalmapTexture()
{
	HRESULT hr;

	ComPtr<ID3D11Texture2D> tex;
	D3D11_TEXTURE2D_DESC tdesc;
	D3D11_SUBRESOURCE_DATA tbsd;

	int h = 4;
	int w = 4;
	int *buf = new int[h*w];
	for (int i = 0; i < h; i++)
	{
		for (int j = 0; j < w; j++)
		{
			buf[i*w + j] = 0xffff8080;
		}
	}

	tbsd.pSysMem = (void *)buf;
	tbsd.SysMemPitch = w * 4;
	tbsd.SysMemSlicePitch = w*h * 4; // Not needed since this is a 2d texture

	tdesc.Width = w;
	tdesc.Height = h;
	tdesc.MipLevels = 1;
	tdesc.ArraySize = 1;

	tdesc.SampleDesc.Count = 1;
	tdesc.SampleDesc.Quality = 0;
	tdesc.Usage = D3D11_USAGE_DEFAULT;
	tdesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	tdesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;

	tdesc.CPUAccessFlags = 0;
	tdesc.MiscFlags = 0;

	if (FAILED(DX11::Device->CreateTexture2D(&tdesc, &tbsd, &tex)))
		return;

	delete[] buf;

	hr = DX11::Device->CreateShaderResourceView(tex.Get(), NULL, myDefaultNormalMapResource.ReleaseAndGetAddressOf());
	if (FAILED(hr))
	{
		return;
	}
}

void Tga::TextureManager::Init()
{
	CreateErrorSquareTexture();
	
	myWhiteSquareTexture = std::make_unique<Texture>();
	myWhiteSquareTexture->myPath = L"WhiteSquare";

	ComPtr<ID3D11ShaderResourceView> resource = CreateWhiteSquareTexture();

	myWhiteSquareTexture->SetShaderResourceView(resource.Get());
	myWhiteSquareTexture->mySize = Vector2f(0.3f, 0.3f);
	myWhiteSquareTexture->myImageSize = Vector2f(512, 512);

	CreateDefaultNormalmapTexture();
}

void Tga::TextureManager::ReleaseTexture(Texture* aTexture)
{
	aTexture->SetShaderResourceView(myFailedResource.Get());
}

void Tga::TextureManager::Update()
{
}
