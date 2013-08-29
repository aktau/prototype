/**
 * This file is part of prototype.
 *
 * (c) 2013 Nicolas Hillegeer <nicolas@hillegeer.com>
 *
 * For the full copyright and license information, please view the LICENSE
 * file that was distributed with the source code.
 */

#include "util.h"

#define GL_SHADER_ERROR(shader) \
    if (DEBUG_TEST) { \
        assert(gfxCheckShader(shader)); \
    }

#define GL_PROGRAM_ERROR(program) \
    if (DEBUG_TEST) { \
        assert(gfxCheckShaderProgram(program)); \
    }

static int gfxCheckShader(GLuint shader);
static int gfxCheckShaderProgram(GLuint program);

void gfxLoadShaderFromFile(struct gfxShaderProgram *shader, const char *vertfile, const char *fragfile) {
    GLchar * const vertsrc = (GLchar * const) loadfile(vertfile);
    GLchar * const fragsrc = (GLchar * const) loadfile(fragfile);

    gfxLoadShader(shader, vertsrc, fragsrc);

    zfree(vertsrc);
    zfree(fragsrc);
}

void gfxLoadShader(struct gfxShaderProgram *shader, const char *vertsrc, const char *fragsrc) {
    trace("vertex shader: \n%s\n", vertsrc);
    trace("fragment shader: \n%s\n", fragsrc);

    /* create and compile shaders */
    GLuint vert = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vert, 1, &vertsrc, NULL);
    glCompileShader(vert);

    GL_SHADER_ERROR(vert);
    GL_ERROR("create and compile vertex shader");

    GLuint frag = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(frag, 1, &fragsrc, NULL);
    glCompileShader(frag);

    GL_SHADER_ERROR(frag);
    GL_ERROR("create and compile fragment shader");

    /* create and link program */
    GLuint program = glCreateProgram();

    glAttachShader(program, vert);
    glAttachShader(program, frag);

    GL_ERROR("attach shaders");

    glBindAttribLocation(program, GFX_VERTEX, "in_position");
    glBindAttribLocation(program, GFX_NORMAL, "in_normal");
    glBindAttribLocation(program, GFX_TEX_COORD, "in_texcoord");
    glBindAttribLocation(program, GFX_COLOR, "in_color");
    glBindAttribLocation(program, GFX_TANGENT, "in_tangent");

    GL_ERROR("bind attrib locations");

    glLinkProgram(program);

    GL_PROGRAM_ERROR(program);
    GL_ERROR("link shader program");

    glUseProgram(program);

    GL_ERROR("use shader program");

    /* cleanup */
    glDetachShader(program, vert);
    glDetachShader(program, frag);

    glDeleteShader(vert);
    glDeleteShader(frag);

    shader->id = program;
}

void gfxDestroyShader(struct gfxShaderProgram *shader) {
    glUseProgram(0);
    glDeleteProgram(shader->id);

    GL_ERROR("delete shader program");
}

static int gfxCheckShader(GLuint shader) {
    GLint compiled = 0, length;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);

    if (compiled == GL_FALSE) {
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length);

        if (length > 0) {
            char *log = zmalloc((size_t) length);
            glGetShaderInfoLog(shader, length, &length, log);

            trace("[ERROR] could not compile shader correctly: %s\n", log);

            zfree(log);
        }

        return 0;
    }

    return 1;
}

static int gfxCheckShaderProgram(GLuint program) {
    GLint linked = 0, length = 0;
    glGetProgramiv(program, GL_LINK_STATUS, &linked);

    if (linked == GL_FALSE) {
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &length);

        if (length > 0) {
            /* length includes the NULL character */
            char *log = zmalloc((size_t) length);
            glGetProgramInfoLog(program, length, &length, log);

            trace("[ERROR] could not link correctly: %s\n", log);

            zfree(log);
        }

        return 0;
    }

    return 1;
}
