#pragma once
#include <tge/EngineDefines.h>

#ifdef USE_VIDEO
#ifdef __cplusplus
extern "C"
{
#endif // __cplusplus
#include "ffmpeg-2.0/libavcodec/avcodec.h"
#include "ffmpeg-2.0/libavdevice/avdevice.h"
#include "ffmpeg-2.0/libavfilter/avfilter.h"
#include "ffmpeg-2.0/libavformat/avformat.h"
#include "ffmpeg-2.0/libavformat/avio.h"
#include "ffmpeg-2.0/libavutil/avutil.h"
#include "ffmpeg-2.0/libpostproc/postprocess.h"
#include "ffmpeg-2.0/libswresample/swresample.h"
#include "ffmpeg-2.0/libswscale/swscale.h"
#include <wtypes.h>
#ifdef __cplusplus
} // end extern "C".
#endif // __cplusplus

#include <tge/videoplayer/VideoAudio.h>
enum VideoError {
	VideoError_NoError = 0,
	VideoError_WrongFormat,
	VideoError_FileNotFound,
};

namespace Tga {

	struct WavData {
		short channels;
		int sampleRate;
		int sampleSize; // in bytes
		unsigned char* data; // packed PCM sample data
		int mySize = 0;
	};

	class VideoPlayer {
	public:
		VideoPlayer();
		~VideoPlayer();

		VideoError Init(const char* aPath, bool aPlayAudio);

		bool DoFirstFrame();
		void Stop();
		void RestartStream();

		double GetFps();
		void Updateound();
		bool Update(unsigned int*& aBuffer, unsigned int aSizeX = 0, unsigned int aSizeY = 0);

		int GrabNextFrame();
		inline AVFrame* GetAvVideoFrame() const { return myAVVideoFrame; }
	private:
		void PlayAudioStream(const WavData& wav);

	private:
		AVCodec* myVideoCodec;

		AVCodecContext* myVideoCodecContext;
		AVCodecContext* myAudioCodecContext;
		SwsContext* mySwsVideoContext;
		SwrContext* mySwsAudioContext;
		AVFormatContext* myVideoFormatContext;

		AVFrame* myAVVideoFrame;
		AVFrame* myAVVideoFrameBGR;
		AVFrame* myAVAudioFrame;
		AVPacket            myAVPacket;
		std::string         myFileName;
		uint8_t* myUIBuffer;
		int                 myReturnResult;
		int                 myFrameCount = 0;
		int                 myGotFrame;
		int                 myDecodedBytes;
		int                 myVideoStreamIndex;
		int                 myNumberOfBytes;
		AVStream* myAudioStream = NULL;
		WavData				myWavData;
		WaveOut* myAudioOutput;

		bool myAudioInitialized = false;
	};
}
#endif