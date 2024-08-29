#include <stdio.h>
#include <string.h>

#include <AL/al.h>
#include <AL/alc.h>

#include "errors.h"


/**
 * A helper function for printing error messages.
 * @param message The type of error that occurred.
 * @param context What was being done when the error occurred.
 * @param details Additional details about the error that occurred.
 * @return The number of characters printed to stderr.
 */
int printErrorMessage(const char* message, const char* context,
                      const char* details) {
    if (message == NULL) {
        return fprintf(stderr, "Unknown error occurred!\n");
    }
    if (context != NULL) {
        return (
            details != NULL ?
            fprintf(stderr, "%s in %s: %s!\n", message, context, details) :
            fprintf(stderr, "%s in %s!\n", message, context)
        );
    } else {
        return (
            details != NULL ?
            fprintf(stderr, "%s: %s!\n", message, details) :
            fprintf(stderr, "%s!\n", message)
        );
    }
}


/**
 * A helper function for printing OpenAL error messages.
 * @param error The error code as returned by alGetError()
 * @param context What was being done when the error occurred.
 */
int printAlError(ALenum error, const char* context) {
    char errorText[64];

    switch (error) {
        case AL_NO_ERROR:
            return 0;  // don't print anything if no error
        case AL_INVALID_NAME:
            strncpy(errorText, "Invalid object ID", 64);
            break;
        case AL_INVALID_ENUM:
            strncpy(errorText, "Invalid enumeration value", 64);
            break;
        case AL_INVALID_VALUE:
            strncpy(errorText, "Invalid value", 64);
            break;
        case AL_INVALID_OPERATION:
            strncpy(errorText, "Invalid operation", 64);
            break;
        case AL_OUT_OF_MEMORY:
            strncpy(errorText, "Not enough memory for OpenAL", 64);
            break;
        default:
            return printErrorMessage(NULL, context, NULL);
    }
    
    return printErrorMessage("AL error", context, errorText);
}


/**
 * A helper function for printing OpenAL context error messages.
 * @param error The error code as returned by alcGetError()
 * @param context What was being done when the error occurred.
 */
int printAlcError(ALCenum error, const char* context) {
    char errorText[64];

    switch (error) {
        case ALC_NO_ERROR:
            return 0;  // don't print anything if no error
        case ALC_INVALID_ENUM:
            strncpy(errorText, "Invalid enumeration value", 64);
            break;
        case ALC_INVALID_VALUE:
            strncpy(errorText, "Invalid value", 64);
            break;
        case ALC_INVALID_DEVICE:
            strncpy(errorText, "Invalid device", 64);
            break;
        case ALC_INVALID_CONTEXT:
            strncpy(errorText, "Invalid context", 64);
            break;
        case ALC_OUT_OF_MEMORY:
            strncpy(errorText, "Not enough memory for OpenAL", 64);
            break;
        default:
            return printErrorMessage(NULL, context, NULL);
    }
    
    return printErrorMessage("ALC error", context, errorText);
}


/* errors.c
 *
 * a collection of helper functions that can help display details about the
 * errors encountered during runtime of an OpenAL program.
 *
 * build with:
 * gcc -c errors.c
 */
