/*
This class will store a texture bound to DX11
*/
#pragma once
#include <tge/EngineDefines.h>

#ifdef USE_VIDEO
namespace Tga
{
	enum class VideoStatus
	{
		Idle,
		Playing,
		ReachedEnd
	};

	class Video
	{
	public:
		Video();
		~Video();
		bool Init(const char* aPath, bool aPlayAudio = false);
		void Play(bool aLoop = false);
		void Pause();
		void Stop();

		void Update(float aDelta);

		/* Will return false if there was something wrong with the load of the video */
		void Restart();
		
		class TextureResource* GetTexture() const { return myTexture; }

		Vector2i GetVideoSize() const { return mySize; }
		VideoStatus GetStatus() const { return myStatus; }
	private:
		class TextureResource* myTexture;
		class VideoPlayer* myPlayer;

		ComPtr<ID3D11ShaderResourceView> myShaderResource;
		ComPtr<ID3D11Texture2D> myD3DTexture;
		
		int *myBuffer;

		Vector2i mySize;

		float myUpdateTime;
		VideoStatus myStatus;
		bool myWantsToPlay;
		bool myIsLooping;

		int myPowerSizeX;
		int myPowerSizeY;
	};
}
#endif