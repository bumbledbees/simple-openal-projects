#include <stdio.h>

#include <AL/al.h>
#include <AL/alc.h>

#include "errors.h"
#include "tone.h"


#define BUFFER_SIZE     2048
#define SAMPLE_RATE     44100
#define N_BUFFERS       3


int main (int argc, char* argv[]) {
    ALuint      buffers[N_BUFFERS];
    ALCcontext* context;
    ALCdevice*  device;
    ALenum      error;
    ALuint      source[1];

    // open output device
    device = alcOpenDevice(NULL);
    if (device == NULL) {
        fprintf(stderr, "Error opening output device!\n");
        return 1;
    }

    {  // create context
    ALCint contextAttributes[] = { ALC_FREQUENCY, SAMPLE_RATE };
    context = alcCreateContext(device, contextAttributes);
    if (context == NULL) {
        printAlcError(alcGetError(device), "creating ALC context");
        alcCloseDevice(device);
        return 1;
    }
    }

    // make context current
    if (!alcMakeContextCurrent(context)) {
        printAlcError(alcGetError(device), "making ALC context current");
        alcDestroyContext(context);
        alcCloseDevice(device);
        return 1;
    }

    // create source object
    alGenSources(1, source);
    if ((error = alGetError()) != AL_NO_ERROR) {
        printAlError(error, "creating audio sources");
        alcMakeContextCurrent(NULL);
        alcDestroyContext(context);
        alcCloseDevice(device);
        return 1;
    }

    // create buffer objects
    alGenBuffers(N_BUFFERS, buffers);
    if ((error = alGetError()) != AL_NO_ERROR) {
        printAlError(error, "creating audio buffers");
        alDeleteSources(1, source);
        alcMakeContextCurrent(NULL);
        alcDestroyContext(context);
        alcCloseDevice(device);
        return 1;
    }

    {  // handle playback
    ALuint      freeBufferQueue[N_BUFFERS];
    ALint       bufferQueueStart = 0;
    ALint       bufferQueueEnd = 2;
    ALint       iSamples = 0;  // index into samples array
    ALint       nProcessedBuffers;
    ALint       nSamples = SAMPLE_RATE * 5;  // 5 seconds of audio
    ALshort     samples[SAMPLE_RATE * 5];
    ALint       sourceState;

    // fill the sample array with a 5 second long 440 Hz sine wave
    if (generateSine(samples, nSamples, SAMPLE_RATE, 440.0, 5) < nSamples) {
        fprintf(stderr, "Error generating sine wave for sample buffer."); 
        nSamples = 0;
    }

    // mark all buffers as free for use
    for (int i = 0; i < N_BUFFERS; i++) {
        freeBufferQueue[i] = buffers[i];
    }

    // initialize sourceState (state should be AL_INITIAL)
    alGetSourcei(*source, AL_SOURCE_STATE, &sourceState);
    if ((error = alGetError()) != AL_NO_ERROR) {
        printAlError(error, "getting source state");
        nSamples = 0;
    }
    
    do {
        // check number of processed buffers
        alGetSourcei(*source, AL_BUFFERS_PROCESSED, &nProcessedBuffers);
        if ((error = alGetError()) != AL_NO_ERROR) {
            printAlError(error, "getting number of processed buffers");
            alSourceStop(*source);
            continue;
        }

        // unqueue processed buffers
        if (nProcessedBuffers > 0) {
            ALuint unqueuedBuffers[N_BUFFERS];

            alSourceUnqueueBuffers(*source, nProcessedBuffers,
                                   unqueuedBuffers);
            if ((error = alGetError()) != AL_NO_ERROR) {
                printAlError(error, "unqueueing processed buffers");
                alSourceStop(*source);
                continue;
            }

            // re-insert unqueued buffers into the free buffer queue
            for (int i = 0; i < nProcessedBuffers; i++) {
                freeBufferQueue[bufferQueueEnd] = unqueuedBuffers[i];
                bufferQueueEnd = (bufferQueueEnd + 1) % N_BUFFERS;
            }
        }

        // load samples into free buffers then queue them
        while (bufferQueueEnd != bufferQueueStart && iSamples < nSamples) {
            ALuint  buffer;
            ALint   bytesToWrite;
            ALint   samplesToWrite;

            // retrieve the first buffer from the free buffer queue
            buffer = freeBufferQueue[bufferQueueStart];
            bufferQueueStart = (bufferQueueStart + 1) % N_BUFFERS;

            samplesToWrite = (((iSamples + BUFFER_SIZE) > nSamples) ?
                              nSamples - iSamples : BUFFER_SIZE);
            bytesToWrite = sizeof(ALshort) * samplesToWrite;
            iSamples += samplesToWrite;

            // load sample data into buffer
            alBufferData(buffer, AL_FORMAT_MONO16, samples + iSamples,
                         bytesToWrite, SAMPLE_RATE);
            if ((error = alGetError()) != AL_NO_ERROR) {
                printAlError(error, "buffering audio data");
                alSourceStop(*source);
                break;
            }

            // queue buffer on the source
            alSourceQueueBuffers(*source, 1, &buffer);
            if ((error = alGetError()) != AL_NO_ERROR) {
                printAlError(error, "queueing buffers");
                alSourceStop(*source);
                break;
            }
            
            // if playback hasn't started yet, start it.
            if (sourceState == AL_INITIAL) {
                alSourcePlay(*source);
                if ((error = alGetError()) != AL_NO_ERROR) {
                    printAlError(error, "playing audio from source");
                    alSourceStop(*source);
                    break;
                }
            }
        }

        // update the value of sourceState variable
        alGetSourcei(*source, AL_SOURCE_STATE, &sourceState);
        if ((error = alGetError()) != AL_NO_ERROR) {
            printAlError(error, "getting source state");
            alSourceStop(*source);
            continue;
        }
    } while (iSamples < nSamples && sourceState != AL_STOPPED);
    }

    // cleanup
    alDeleteBuffers(N_BUFFERS, buffers);
    alDeleteSources(1, source);
    alcMakeContextCurrent(NULL);
    alcDestroyContext(context);
    alcCloseDevice(device);

    return 0;
}


/**
 * buffering.c
 *
 * an OpenAL program that uses multiple buffers to play audio by queueing
 * buffers on a source. this is useful for cases where you want to "stream"
 * audio data to a source uninterrupted. this example uses three buffers and
 * a crudely implemented queue to keep track of which buffers can be used.
 *
 * thanks to this post https://stackoverflow.com/a/215575/11659424 for helping
 * me figure out how to implement said queue
 *
 * build errors.o and tone.o then build & run with:
 * gcc -c buffering.c
 * gcc -o buffering buffering.o errors.o tone.o -lm -lopenal
 * ./buffering
 */
