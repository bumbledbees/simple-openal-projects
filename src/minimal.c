#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include <AL/al.h>
#include <AL/alc.h>

#define SAMPLE_RATE 44100


int main (int argc, char* argv[]) {
    // open output device
    ALCdevice* device = alcOpenDevice(NULL);  // NULL = select preferred output device

    // create context
    ALCint contextAttributes[] = { ALC_FREQUENCY, SAMPLE_RATE };
    ALCcontext* context = alcCreateContext(device, contextAttributes);
    alcMakeContextCurrent(context);

    // create source object
    ALuint source[1];
    alGenSources(1, source);

    // create buffer object
    ALuint buffer[1];
    alGenBuffers(1, buffer);

    // generate samples
    ALsizei size = sizeof(ALshort) * SAMPLE_RATE;
    ALshort* data = (ALshort*) malloc(size);
    ALfloat interval = 2 * M_PI * 440.0;
    for (int i = 0; i < SAMPLE_RATE; i++) {
        float time = (float) i / (float) SAMPLE_RATE;
        data[i] = (ALshort) (sin(time * interval) * INT16_MAX);
    }

    // buffer the data
    alBufferData(buffer[0], AL_FORMAT_MONO16, data, size, SAMPLE_RATE);
    free(data);

    // play the audio
    alSourcei(source[0], AL_BUFFER, buffer[0]);  // attach the buffer to the source
    alSourcePlay(source[0]);  // play the audio from the buffer

    // wait until finished 
    ALint sourceState;
    do {
        alGetSourcei(source[0], AL_SOURCE_STATE, &sourceState);
    } while (sourceState != AL_STOPPED);

    // cleanup
    alDeleteBuffers(1, buffer);
    alDeleteSources(1, source);
    alcMakeContextCurrent(NULL);
    alcDestroyContext(context);
    alcCloseDevice(device);

    return 0;
}

/* minimal.c
 *
 * a program that uses OpenAL to produce a 1 second 440Hz sine wave. it is
 * worth noting that this code was written with brevity and simplicity in mind,
 * forgoing things like error checking and other good practices in the process.
 * it is intended less as an example of what should be done and more of a
 * minimally functional demo of what you need to do to make a sound.
 *
 * build & run with:
 * gcc minimal.c -o minimal -lm -lopenal; ./minimal
 */
