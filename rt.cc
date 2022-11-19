#include <iostream>
#include "RtAudio.h"

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

  // unsigned int *bytes = (unsigned int *) data;
  for(int i = 0; i < nBufferFrames; i++) {
    *out = *in;
    out++;
    *out = *in;
    in++;
    out++;
  }
  // memcpy( outputBuffer, inputBuffer, *bytes );
  return 0;
}

int main() {
  RtAudio audio;

  unsigned int deviceCount = audio.getDeviceCount();

  RtAudio::DeviceInfo info;
  for(unsigned int i = 0; i < deviceCount; i++) {
    info = audio.getDeviceInfo(i);
    std::cout << "Device " << i << ": " << info.name << std::endl;
  }

  unsigned int bufferFrames = 256;
  unsigned int sampleRate = 44100;
  unsigned int bufferBytes = bufferFrames * sizeof( MY_TYPE );

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
    &oParams, &iParams, FORMAT, sampleRate, &bufferFrames, &inout, (void *)&bufferBytes, &options
  );

  std::cout << audio.isStreamOpen() << std::endl;

  audio.startStream();

  char input;
  std::cout << "\nRunning ... press <enter> to quit (buffer frames = " << bufferFrames << ").\n";
  std::cin.get(input);

  return 0;
}