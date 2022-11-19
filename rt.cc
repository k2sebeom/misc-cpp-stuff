#include <iostream>
#include "RtAudio.h"
#include <sndfile.hh>
extern "C" {
  #include <libavformat/avformat.h>
  #include <libavcodec/avcodec.h>
}


typedef float MY_TYPE;
#define FORMAT RTAUDIO_FLOAT32

double streamTimePrintIncrement = 1.0; // seconds
double streamTimePrintTime = 1.0; // seconds

int inout( void *outputBuffer, void *inputBuffer, unsigned int nBufferFrames,
           double streamTime, RtAudioStreamStatus status, void *data )
{
  // Since the number of input and output channels is equal, we can do
  // a simple buffer copy operation here.
  if ( status ) std::cout << "Stream over/underflow detected." << std::endl;

  if ( streamTime >= streamTimePrintTime ) {
    std::cout << "streamTime = " << streamTime << std::endl;
    streamTimePrintTime += streamTimePrintIncrement;
  }

  float *out = (float*)outputBuffer;
  float *in = (float*)inputBuffer;
  SndfileHandle *file = (SndfileHandle *) data;
  file->readf(out, nBufferFrames);

  for(int i = 0; i < nBufferFrames; i++) {
    *out *= 0.5;
    *out += *in * 0.5;
    out++;
    *out *= 0.5;
    *out += *in * 0.5;
    in++;
    out++;
  }
  return 0;
}

int main(int argc, const char *argv[]) {
  // static float buffer[1024];
  AVFormatContext* format = avformat_alloc_context();

  avformat_open_input(&format, argv[1], NULL, NULL);

  std::cout << format->iformat->name << std::endl;

  // AVCodec *pCodec = NULL;
  // AVCodecParameters *pCodecParameters = NULL;

  for (int i = 0; i < format->nb_streams; i++) {
    AVCodecParameters *pCodecParameters = format->streams[i]->codecpar;
    std::cout << "Stream " << i << ": " << pCodecParameters->ch_layout.nb_channels << std::endl;
  }

  avformat_close_input(&format);
  avformat_free_context(format);



  SndfileHandle file;
  avformat_free_context(format);

  file = SndfileHandle("DM.wav");

  RtAudio audio;

  unsigned int deviceCount = audio.getDeviceCount();

  RtAudio::DeviceInfo info;
  for(unsigned int i = 0; i < deviceCount; i++) {
    info = audio.getDeviceInfo(i);
    std::cout << "Device " << i << ": " << info.name << std::endl;
  }

  unsigned int bufferFrames = 256;
  unsigned int sampleRate = 44100;
  // unsigned int bufferBytes = bufferFrames * sizeof( MY_TYPE );

  RtAudio::StreamParameters iParams, oParams;

  iParams.deviceId = audio.getDefaultInputDevice();
  oParams.deviceId = audio.getDefaultOutputDevice();
  iParams.nChannels = 1;
  oParams.nChannels = 2;
  iParams.firstChannel = 0;
  oParams.firstChannel= 0;
  
  RtAudio::StreamOptions options;
  // options.flags = RTAUDIO_NONINTERLEAVED;

  audio.openStream(
    &oParams, &iParams, FORMAT, sampleRate, &bufferFrames, &inout, (void *)&file, &options
  );

  std::cout << audio.isStreamOpen() << std::endl;

  audio.startStream();

  char input;
  std::cout << "\nRunning ... press <enter> to quit (buffer frames = " << bufferFrames << ").\n";
  std::cin.get(input);

  return 0;
}