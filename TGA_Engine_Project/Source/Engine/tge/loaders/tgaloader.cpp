#include "stdafx.h"
#include <cstdio>
#include <cassert>
#include <memory>
#include <iostream>
#define WIN32_LEAN_AND_MEAN 
#define NOMINMAX 
#include <windows.h>
#include <tge/loaders/tgaloader.h>

Tga32::Tga32()
{
}
Tga32::~Tga32()
{
}
Tga32::Image::Image()
{
}
Tga32::Image::~Image()
{
	delete myImage;
}


void FlipImageDataFast(const Tga32::TgaHeader& header, unsigned char* source, unsigned char* destination)
{
	int height = header.myHeight;
	const unsigned short stride = header.myWidth * 4;

	unsigned char* row = (unsigned char*)malloc(stride);
	unsigned char* low = source;
	unsigned char* high = &source[(height - 1) * stride];

	for (; low < high; low += stride, high -= stride)
	{
		memcpy(row, low, stride);
		memcpy(low, high, stride);
		memcpy(high, row, stride);
	}
	free(row);
	row = NULL;

	for (int i = 0; i < height * stride; i += 4)
	{
		unsigned char temp = std::move(source[i]);
		source[i] = std::move(source[i + 2]);
		source[i + 2] = std::move(temp);
	}

	memcpy(destination, source, sizeof(source));
}




Tga32::Image *Tga32::Load( const wchar_t* aName )
{
	FILE* fp = NULL;
	Image* myImage;

	myImage = new Image(); //allocates return object

	_wfopen_s(&fp, aName, L"rb");
	assert(fp != NULL);

	TgaHeader header;
	fread(&header, sizeof(header), 1, fp); // loads the header

	myImage->myWidth = header.myWidth;
	myImage->myHeight = header.myHeight;

	if( header.myBpp == 32 )
	{
		myImage->myImage = (unsigned char*)malloc(myImage->myHeight * myImage->myWidth * 4 + myImage->myWidth * 4);

		unsigned char* buffer;
		buffer = myImage->myImage + myImage->myWidth * 4;

		fread(buffer, myImage->myHeight * myImage->myWidth * 4, 1, fp); // loads the image

		fclose(fp);

		FlipImageDataFast(header, buffer, myImage->myImage);

		myImage->myBitDepth=32;
		return(myImage);
	}
	
	return( NULL );
}
