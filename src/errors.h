#ifndef ERRORS_H
#define ERRORS_H
#endif

#include <AL/al.h>
#include <AL/alc.h>


int printAlError(ALenum error, const char* context);
int printAlcError(ALCenum error, const char* context);


/* errors.h
 *
 * since the helper functions defined in errors.c are pretty useful, i figured
 * i'd include a header for them so they can be used elsewhere.
 */
