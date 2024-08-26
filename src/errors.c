#include <stdio.h>
#include <string.h>

#include <AL/al.h>
#include <AL/alc.h>


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


int main (int argc, char* argv[]) {
    ALuint buffer[1];
    ALCcontext* context;
    ALCdevice* device;
    ALenum error;
    ALuint source[1];

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
        alcMakeContextCurrent(NULL);
        alcDestroyContext(context);
        alcCloseDevice(device);
        return 1;
    }

    // you'd do your audio stuff here

    // cleanup:
    alDeleteBuffers(1, buffer);
    alDeleteSources(1, source);
    alcMakeContextCurrent(NULL);
    alcDestroyContext(context);
    alcCloseDevice(device);

    printf("The program finished without errors! :)\n");
    return 0;
}


/* errors.c
 *
 * a demonstration of how to check for errors while writing OpenAL code, along
 * with some sample definitions of functions to help display details about the
 * errors encountered. try commenting / uncommenting sections of code to mess
 * around with different error states.
 *
 * build & run with:
 * gcc errors.c -o errors -lopenal; ./errors
 */
