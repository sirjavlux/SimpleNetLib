/*
This class will store a texture bound to DX11
*/
#pragma once
#include <tge/EngineDefines.h>
#define WIN32_LEAN_AND_MEAN 
#define NOMINMAX 
#include <windows.h>
#include <mmeapi.h>

#ifdef USE_VIDEO
namespace Tga
{
	class WaveBuffer 
	{
	public:
		~WaveBuffer();
		BOOL Init(HWAVEOUT hWave, int Size);
		BOOL Write(PBYTE pData, int nBytes, int& BytesWritten);
		void Flush();
	private:
		WAVEHDR      m_Hdr;
		HWAVEOUT     m_hWave;
		int          m_nBytes;
	};

	class WaveOut {
	public:
		WaveOut(LPCWAVEFORMATEX Format, int nBuffers, int BufferSize);
		~WaveOut();
		void Write(PBYTE Data, int nBytes);
		void Flush();
		void Wait();
		void Reset();
	private:
		const HANDLE   m_hSem;
		const int      m_nBuffers;
		int            m_CurrentBuffer;
		BOOL           m_NoBuffer;
		WaveBuffer   *m_Hdrs;
		HWAVEOUT       m_hWave;
	};

	

}
#endif