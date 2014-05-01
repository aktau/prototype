/**
 * This file is part of prototype.
 *
 * (c) 2013 Nicolas Hillegeer <nicolas@hillegeer.com>
 *
 * For the full copyright and license information, please view the LICENSE
 * file that was distributed with the source code.
 */

#include "util.h"

#include <stdio.h>
#include <sys/stat.h>

#include "zmalloc.h"

const char *wfGlErrorString(GLenum error) {
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

const char *wfGlFbErrorString() {
    switch (glCheckFramebufferStatus(GL_FRAMEBUFFER)) {
        case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:         return "framebuffer incomplete attachment";
        case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT: return "framebuffer incomplete missing attachment";
        case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER:        return "framebuffer incomplete draw buffer";
        case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER:        return "framebuffer incomplete read buffer";
        case GL_FRAMEBUFFER_UNSUPPORTED:                   return "framebuffer unsupported";
        case GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE:        return "framebuffer incomplete multisample";
        case GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS:      return "framebuffer incomplete layer targets";
        case GL_FRAMEBUFFER_UNDEFINED:                     return "framebuffer undefined";

        default: return NULL;
    }
}

/* idea and implementation by @rygorous */
void wfGlCheckLeaks(void) {
    /* better idea would be to keep track of assigned names. */
    GLuint max_id = 10000;
    GLuint id;

    /* if brute force doesn't work, you're not applying it hard enough */
    for (id = 1; id <= max_id; ++id) {
#define CHECK( type ) if ( glIs##type( id ) ) trace("GLX: leaked " #type " handle 0x%x\n", (unsigned int) id )
        CHECK( Texture );
        CHECK( Buffer );
        CHECK( Framebuffer );
        CHECK( Renderbuffer );
        CHECK( VertexArray );
        CHECK( Shader );
        CHECK( Program );
        CHECK( ProgramPipeline );
        CHECK( Query );
#undef CHECK
    }

    trace("leak check done\n");
}

/* caller is responsible for freeing the buffer if not NULL */
char *loadfile(const char *filename) {
    struct stat stats;

    char *buffer = NULL;
    FILE *file   = NULL;
    size_t size  = 0;
    size_t read  = 0;
    int res      = 0;

    res = stat(filename, &stats);
    ERROR_HANDLE(res == -1, errno, "could not stat %s", filename);

    trace("size of file %s: %llu bytes\n", filename, stats.st_size);

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
