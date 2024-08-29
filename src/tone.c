#include <math.h>
#include <limits.h>
#include <stdio.h>

#include <AL/al.h>

#include "tone.h"


/**
 * A helper function that generates a sine wave of a given frequency. If the
 *   specified duration would require more space than the buffer can hold, the
 *   buffer is filled with as many samples as possible.
 * @param buffer The location in memory where the sample data will be written.
 * @param bufSize The number of samples the buffer can hold.
 * @param sampleRate The sample rate to use when filling the buffer, in Hz.
 * @param pitch The frequency of the sine wave, in Hz.
 * @param duration The duration of the sound, in seconds.
 * @return The number of bytes written to `buffer`.
 */
ALint generateSine(ALshort* buffer, ALsizei bufSize, ALint sampleRate,
                   ALfloat pitch, ALfloat duration) {
    ALfloat interval = 2 * M_PI * pitch;
    ALint nSamples = (ALint) round(duration * (ALfloat) sampleRate);
    
    if (buffer == NULL) { return 0; }
    if (nSamples > bufSize) { nSamples = bufSize; }
    for (int i = 0; i < nSamples; i++) {
        float time = (float) i / sampleRate;
        buffer[i] = (ALshort) (sin(time * interval) * SHRT_MAX);
    }
    return nSamples;
}


/**
 * tone.c
 * 
 * a collection of functions (hopefully eventualy; right now it's one function)
 * that generate samples of specific waveforms for use with OpenAL.
 *
 * build with:
 * gcc -c tone.c
 */
