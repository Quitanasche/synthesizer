#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "portaudio.h"

#define SAMPLE_RATE (44100)
#define AMPLITUDE (0.5)
#define FREQUENCY (440.0)

typedef struct {
    float phase;
} paTestData;

static int paCallback(const void* inputBuffer, void* outputBuffer,
                      unsigned long framesPerBuffer,
                      const PaStreamCallbackTimeInfo* timeInfo,
                      PaStreamCallbackFlags statusFlags,
                      void* userData) {
    paTestData* data = (paTestData*)userData;
    float* out = (float*)outputBuffer;
    unsigned int i;

    (void)inputBuffer; /* Prevent unused variable warnings. */

    for (i = 0; i < framesPerBuffer; i++) {
        *out++ = AMPLITUDE * sinf(data->phase);
        data->phase += 2.0f * M_PI * FREQUENCY / SAMPLE_RATE;
        if (data->phase > 2.0f * M_PI) {
            data->phase -= 2.0f * M_PI;
        }
    }

    return paContinue;
}

int main(void) {
    PaStream* stream;
    PaError err;
    paTestData data;

    data.phase = 0.0f;

    err = Pa_Initialize();
    if (err != paNoError) {
        fprintf(stderr, "PortAudio error: %s\n", Pa_GetErrorText(err));
        return 1;
    }

    err = Pa_OpenDefaultStream(&stream, 0, 1, paFloat32, SAMPLE_RATE, 256,
                               paCallback, &data);
    if (err != paNoError) {
        fprintf(stderr, "PortAudio error: %s\n", Pa_GetErrorText(err));
        Pa_Terminate();
        return 1;
    }

    err = Pa_StartStream(stream);
    if (err != paNoError) {
        fprintf(stderr, "PortAudio error: %s\n", Pa_GetErrorText(err));
        Pa_CloseStream(stream);
        Pa_Terminate();
        return 1;
    }

    printf("Press Enter to stop the sound...\n");
    getchar();

    err = Pa_CloseStream(stream);
    if (err != paNoError) {
        fprintf(stderr, "PortAudio error: %s\n", Pa_GetErrorText(err));
    }

    Pa_Terminate();
    return 0;
}
