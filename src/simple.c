#include <stdio.h>

#include <AL/al.h>
#include <AL/alc.h>

#include "errors.h"
#include "tone.h"

#define SAMPLE_RATE 44100


int main (int argc, char* argv[]) {
    ALuint      buffer[1];
    ALCcontext* context;
    ALCdevice*  device;
    ALenum      error;
    ALint       nSamples = SAMPLE_RATE;
    ALshort     samples[SAMPLE_RATE];
    ALuint      source[1];

    // this opens the device normally:
    device = alcOpenDevice(NULL);

    // doing this instead results in device being equal to NULL:
    // device = alcOpenDevice("asdfghjkl");

    if (device == NULL) {
        fprintf(stderr, "Error opening output device!\n");
        return 1;
    }

    // doing this before creating the context results in ALC_INVALID_DEVICE:
    // alcCloseDevice(device); device = NULL;

    // this creates the context normally:
    context = alcCreateContext(device, NULL);

    if (context == NULL) {
        printAlcError(alcGetError(device), "creating ALC context");
        alcCloseDevice(device);
        return 1;
    }

    // doing this results in ALC_INVALID_CONTEXT on device NULL:
    // alcDestroyContext(context);

    if (!alcMakeContextCurrent(context)) {
        printAlcError(alcGetError(NULL), "making ALC context current");
        printAlcError(alcGetError(device), "making ALC context current");
        alcDestroyContext(context);
        alcCloseDevice(device);
        return 1;
    }

    // doing this before generating sources results in AL_INVALID_OPERATION:
    // alcMakeContextCurrent(NULL);

    // this creates the source normally:
    alGenSources(1, source);

    // doing this instead results in AL_INVALID_VALUE:
    // alGenSources(-1, source);

    if ((error = alGetError()) != AL_NO_ERROR) {
        printAlError(error, "creating audio sources");
        alcMakeContextCurrent(NULL);
        alcDestroyContext(context);
        alcCloseDevice(device);
        return 1;
    }
    
    // doing this before generating buffers results in AL_INVALID_OPERATION:
    // alcMakeContextCurrent(NULL);

    // doing this generates buffers normally:
    alGenBuffers(1, buffer);

    // doing this instead results in AL_INVALID_VALUE:
    // alGenBuffers(-1, buffer);

    if ((error = alGetError()) != AL_NO_ERROR) {
        printAlError(error, "creating audio buffers");
        alDeleteSources(1, source);
        alcMakeContextCurrent(NULL);
        alcDestroyContext(context);
        alcCloseDevice(device);
        return 1;
    }

    // fill sample array with 1 second long 440 Hz sine wave
    if (generateSine(samples, nSamples, SAMPLE_RATE, 440.0, 1) < nSamples) {
        fprintf(stderr, "Error generating sine wave for sample buffer.");
        alDeleteSources(1, source);
        alcMakeContextCurrent(NULL);
        alcDestroyContext(context);
        alcCloseDevice(device);
        return 1;
    }

    // buffer the data
    alBufferData(*buffer, AL_FORMAT_MONO16, samples,
                 sizeof(ALshort) * nSamples, SAMPLE_RATE);
    if ((error = alGetError()) != AL_NO_ERROR) {
        printAlError(error, "buffering audio data");
        alDeleteSources(1, source);
        alcMakeContextCurrent(NULL);
        alcDestroyContext(context);
        alcCloseDevice(device);
        return 1;
    }

    // attach the buffer to the source
    alSourcei(*source, AL_BUFFER, *buffer);
    if ((error = alGetError()) != AL_NO_ERROR) {
        printAlError(error, "attaching buffer to source");
        alDeleteSources(1, source);
        alcMakeContextCurrent(NULL);
        alcDestroyContext(context);
        alcCloseDevice(device);
        return 1;
    }

    // play the audio from the buffer
    alSourcePlay(*source);
    if ((error = alGetError()) != AL_NO_ERROR) {
        printAlError(error, "playing audio from source");
        alDeleteSources(1, source);
        alcMakeContextCurrent(NULL);
        alcDestroyContext(context);
        alcCloseDevice(device);
        return 1;
    }

    // wait until finished 
    {
    ALint sourceState;
    do {
        // get "source state" attribute of source
        alGetSourcei(*source, AL_SOURCE_STATE, &sourceState);
        if ((error = alGetError()) != AL_NO_ERROR) {
            printAlError(error, "getting source state");
            alSourceStop(*source);
        }
    } while (sourceState != AL_STOPPED);
    }

    // cleanup:
    alDeleteBuffers(1, buffer);
    alDeleteSources(1, source);
    alcMakeContextCurrent(NULL);
    alcDestroyContext(context);
    alcCloseDevice(device);

    return 0;
}


/**
 * simple.c
 *
 * a program equivalent to minimal.c that does things like error checking and
 * cleanup using the helper functions from errors.c and tone.c. i don't like
 * all the code duplication and i might write some more helper functions in the
 * future to make that a little nicer but for now, it works.
 *
 * also provided are some comments that demonstrate ways to trigger various
 * error states during runtime, feel free to uncomment those and mess about
 *
 * build errors.o and tone.o then build & run with:
 * gcc -c simple.c
 * gcc -o simple simple.o errors.o tone.o -lm -lopenal
 * ./simple
 */
