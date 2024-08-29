#ifndef ERRORS_H
#define ERRORS_H
#endif

#include <AL/al.h>
#include <AL/alc.h>


int printAlError(ALenum error, const char* context);
int printAlcError(ALCenum error, const char* context);
int printErrorMessage(const char* message, const char* context,
                      const char* details);
