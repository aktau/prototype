#include "util.h"

#include <stdio.h>
#include <sys/stat.h>

#include "zmalloc.h"

const char* wfGlErrorString(GLenum error) {
    switch (error) {
        case GL_NO_ERROR: return "no error";
        case GL_INVALID_ENUM: return "invalid enum";
        case GL_INVALID_VALUE: return "invalid enum";
        case GL_INVALID_OPERATION: return "invalid enum";
        case GL_INVALID_FRAMEBUFFER_OPERATION: return "invalid enum";
        case GL_OUT_OF_MEMORY: return "invalid enum";
        default: return "unknown error type";
    }
}

/* caller is responsible for freeing the buffer if not NULL */
char* loadfile(char *filename) {
    struct stat stats;

    char *buffer = NULL;
    FILE *file   = NULL;
    size_t size  = 0;
    size_t read  = 0;
    int res      = 0;

    res = stat(filename, &stats);
    ERROR_HANDLE(res == -1, errno, "could not stat %s", filename);

    trace("%llu\n", stats.st_size);

    size = (size_t) stats.st_size;

    file = fopen(filename, "rb");
    ERROR_HANDLE(file == NULL, errno, "error while opening file %s", filename);

    /* allocate a buffer large enough for the entire file + terminator */
    buffer = zmalloc(size + 1);

    /* read the file in one go */
    read = fread(buffer, 1, size, file);
    ERROR_HANDLE(read != size, errno, "could not read entire file\n");

    /* NULL terminate */
    buffer[size] = '\0';

    fclose(file);
    file = NULL;

    return buffer;

error:
    zfree(buffer);

    if (file) {
        fclose(file);
    }

    return NULL;
}
