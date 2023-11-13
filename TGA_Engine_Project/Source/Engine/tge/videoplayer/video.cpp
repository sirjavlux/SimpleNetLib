#include "stdafx.h"
#include <tge/videoplayer/video.h>
#include <tge/videoplayer/videoplayer.h>

#ifdef USE_VIDEO

#include <tge/sprite/sprite.h>
#include <tge/graphics/DX11.h>
#include <tge/texture/TextureManager.h>

using namespace Tga;
Tga::Video::Video()
	:myPlayer(nullptr)
{
	myBuffer = nullptr;
	myUpdateTime = 0.0f;
	myStatus = VideoStatus::Idle;
	myWantsToPlay = false;
	myTexture = nullptr;
	myIsLooping = false;
}

Tga::Video::~Video()
{
	if (myTexture)
	{
		delete myTexture;
		myTexture = nullptr;
	}

	if (myBuffer)
	{
		delete[] myBuffer;
	}

	delete myPlayer;
	myPlayer = nullptr;
	
}

void Tga::Video::Play(bool aLoop)
{
	myWantsToPlay = true;
	myIsLooping = aLoop;
}

void Tga::Video::Pause()
{
	myWantsToPlay = false;
}

void Tga::Video::Stop()
{
	myStatus = VideoStatus::Idle;
	myWantsToPlay = false;
	myPlayer->Stop();
}

void Tga::Video::Restart()
{
	myWantsToPlay = true;
	myPlayer->RestartStream();
}
#include <functional>
bool Video::Init(const char* aPath, bool aPlayAudio)
{
	if (myPlayer)
	{
		return false;
	}

	myPlayer = new VideoPlayer();
	if (myPlayer)
	{
		VideoError error = myPlayer->Init(aPath, aPlayAudio);
		if (error == VideoError_WrongFormat)
		{
			ERROR_PRINT("%s %s %s", "Could not load video: ", aPath, ". Wrong format?");
			return false;
		}
		else if (error == VideoError_FileNotFound)
		{
			ERROR_PRINT("%s %s %s", "Could not load video: ", aPath, ". File not found");
			return false;
		}
	}

	if (!myPlayer->DoFirstFrame())
	{
		ERROR_PRINT("%s %s %s", "Video error: ", aPath, ". First frame not found?");
		return false;
	}
	
	mySize.x = myPlayer->GetAvVideoFrame()->width;
	mySize.y = myPlayer->GetAvVideoFrame()->height;

	myPowerSizeX = (int)powf(2, ceilf(logf((float)mySize.x) / logf(2)));
	myPowerSizeY = (int)powf(2, ceilf(logf((float)mySize.y) / logf(2)));

	myBuffer = new int[(myPowerSizeX* myPowerSizeY)];

	myStatus = VideoStatus::Playing;


	if (!myShaderResource)
	{
		D3D11_TEXTURE2D_DESC texture_desc;
		texture_desc.Width = myPowerSizeX;
		texture_desc.Height = myPowerSizeY;
		texture_desc.MipLevels = 1;
		texture_desc.ArraySize = 1;
		texture_desc.MipLevels = 1;
		texture_desc.ArraySize = 1;

		texture_desc.SampleDesc.Count = 1;
		texture_desc.SampleDesc.Quality = 0;
		texture_desc.Usage = D3D11_USAGE_DYNAMIC;
		texture_desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
		texture_desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;

		texture_desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		texture_desc.MiscFlags = 0;

		DX11::Device->CreateTexture2D(&texture_desc, nullptr, myD3DTexture.ReleaseAndGetAddressOf());
		DX11::Device->CreateShaderResourceView(myD3DTexture.Get(), NULL, myShaderResource.ReleaseAndGetAddressOf());

		myTexture = new TextureResource(myShaderResource.Get());
	}

	// Force one frame so we dont get a blank video
	bool wantsToPlay = myWantsToPlay;
	myWantsToPlay = true;
	myUpdateTime = 0.0001f;
	Update(0);
	myUpdateTime = 0.0f;
	myWantsToPlay = wantsToPlay;

	return true;
}

void Video::Update(float aDelta)
{
	if (!myWantsToPlay || !myPlayer)
	{
		return;
	}
	myUpdateTime += aDelta;

	double fps = myPlayer->GetFps();

	myPlayer->Updateound();
	while (myUpdateTime >= 1.0f / fps)
	{
		if (myShaderResource && myD3DTexture)
		{
			int status = myPlayer->GrabNextFrame();
			if (status < 0)
			{
				myStatus = VideoStatus::ReachedEnd;
				if (myIsLooping)
				{
					Restart();
				}
			}

			D3D11_MAPPED_SUBRESOURCE  mappedResource;
			HRESULT result = DX11::Context->Map(myD3DTexture.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
			if (FAILED(result))
			{
				return;
			}

			unsigned int* source = (unsigned int*)(mappedResource.pData);
			myPlayer->Update(source, myPowerSizeX, myPowerSizeY);
			DX11::Context->Unmap(myD3DTexture.Get(), 0);
		}
		myUpdateTime -= 1.0f / static_cast<float>(fps);
	}
}

#endif