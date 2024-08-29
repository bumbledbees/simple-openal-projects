#ifndef TONE_H
#define TONE_H
#endif

#include <AL/al.h>


ALint generateSine(ALshort* buffer, ALsizei bufSize, ALint sampleRate,
                   ALfloat pitch, ALfloat duration);
