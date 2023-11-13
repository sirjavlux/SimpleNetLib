#include "stdafx.h"
#include <tge/audio/audio.h>
#include <tge/audio/audio_out.h>
using namespace Tga;

#pragma comment(lib,"bass.lib")

AudioOut* Audio::myAudioOut = nullptr;
Audio::Audio()
	:myLoadedAudio(-1),
	myChannel(0),
	myVolume(1.0f)
{
	myPosition.Set(0, 0);
}
Audio::~Audio()
{

}

void Audio::Init(const char * aPath, bool aIsLooping)
{
	if (!myAudioOut)
	{
		myAudioOut = new Tga::AudioOut();
	}
	myLoadedAudio = myAudioOut->Load(Tga::Settings::ResolveAssetPath(aPath), aIsLooping);
	
}

void Audio::Play()
{
	if (myLoadedAudio == -1)
	{
		return;
	}
	myAudioOut->Play(myLoadedAudio, myChannel, myVolume);
}

void Tga::Audio::SetVolume(float aVolume)
{
	myVolume = aVolume;
	if (myLoadedAudio == -1)
	{
		return;
	}
	myAudioOut->SetVolume(myChannel, myVolume);
}

void Tga::Audio::SetPosition(Vector2f aPosition)
{
	myPosition = aPosition;
	myAudioOut->SetPosition(myChannel, myPosition);
}

void Audio::Stop(bool aImmediately)
{
	if (myLoadedAudio == -1)
	{
		return;
	}
	myAudioOut->Stop(myLoadedAudio, aImmediately);
}
