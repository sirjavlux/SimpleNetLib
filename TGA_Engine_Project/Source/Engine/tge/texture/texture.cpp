#include "stdafx.h"
#include <tge/texture/Texture.h>
#include <ScreenGrab/ScreenGrab11.h>
#include <tge/texture/TextureManager.h>

Tga::Texture::Texture()
{
	myPath = L"undefined";
	myIsFailedTexture = false;
}

Tga::Texture::~Texture()
{
	if (myIsFailedTexture)
	{
		return;
	}
	myPath = L"undefined";
	myID = 0;
}