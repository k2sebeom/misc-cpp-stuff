#include <iostream>
#include <portaudio.h>

#define SAMPLE_RATE  (44100)
#define FRAMES_PER_BUFFER (64)
#define NUM_SECONDS     (5)
#define SAMPLE_SIZE (1)

int patestCallback( const void *inputBuffer, void *outputBuffer,
                           unsigned long framesPerBuffer,
                           const PaStreamCallbackTimeInfo* timeInfo,
                           PaStreamCallbackFlags statusFlags,
                           void *userData )
{
    /* Cast data passed through stream to our structure. */
    float *out = (float*)outputBuffer;
    unsigned int i;
    float *in = (float*)inputBuffer;
    
    for( i=0; i<framesPerBuffer; i++ )
    {
        *out = *in;
        out++;
        in++;
        // std::cout << *in << std::endl;
    }
    return 0;
}

int main() {
    PaError err = Pa_Initialize();
    if (err != paNoError) {
        std::cout << "Error initializing PA" << std::endl;
        return 1;
    }

    PaStream *stream;
    err = Pa_OpenDefaultStream(&stream,
                                1,
                                1,
                                paFloat32,
                                SAMPLE_RATE,
                                FRAMES_PER_BUFFER,
                                patestCallback,
                                NULL
    );

    err = Pa_StartStream(stream);

    if (err != paNoError) {
        std::cout << "Error starting PA stream" << std::endl;
        return 1;
    }

    // int numBytes = FRAMES_PER_BUFFER * 2 * SAMPLE_SIZE;
    // char *sampleBlock = (char *) malloc( numBytes );
    Pa_Sleep(NUM_SECONDS*1000);
    // for( int i=0; i<(NUM_SECONDS*SAMPLE_RATE)/FRAMES_PER_BUFFER; ++i )
    // {
    //     err = Pa_ReadStream( stream, sampleBlock, FRAMES_PER_BUFFER );
    //     err = Pa_WriteStream( stream, sampleBlock, FRAMES_PER_BUFFER );       
    // }

    err = Pa_StopStream( stream );

    err = Pa_Terminate();
    return 0;
}