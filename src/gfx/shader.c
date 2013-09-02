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

void gfxSetShaderParams(const struct gfxShaderProgram *shader) {
    if (shader->loc.texture0 != -1) glUniform1i(shader->loc.texture0, 0);
    if (shader->loc.texture1 != -1) glUniform1i(shader->loc.texture1, 1);
    if (shader->loc.texture2 != -1) glUniform1i(shader->loc.texture2, 2);
    if (shader->loc.texture3 != -1) glUniform1i(shader->loc.texture3, 3);
}

void gfxLoadShaderFromFile(struct gfxShaderProgram *shader, const char *vertfile, const char *fragfile) {
    GLchar * const vertsrc = (GLchar * const) loadfile(vertfile);
    GLchar * const fragsrc = (GLchar * const) loadfile(fragfile);

    gfxLoadShader(shader, vertsrc, fragsrc);

    zfree(vertsrc);
    zfree(fragsrc);
}

void gfxLoadShader(struct gfxShaderProgram *shader, const char *vertsrc, const char *fragsrc) {
    memset(shader, 0x0, sizeof(struct gfxShaderProgram));

    trace("vertex shader: \n%s\n", vertsrc);
    trace("fragment shader: \n%s\n", fragsrc);

    /* create and compile shaders */
    GLuint vert = glCreateShader(GL_VERTEX_SHADER);
    GLuint frag = glCreateShader(GL_FRAGMENT_SHADER);

    glShaderSource(vert, 1, &vertsrc, NULL);
    glShaderSource(frag, 1, &fragsrc, NULL);

    glCompileShader(vert);

    GL_SHADER_ERROR(vert);
    GL_ERROR("create and compile vertex shader");

    glCompileShader(frag);

    GL_SHADER_ERROR(frag);
    GL_ERROR("create and compile fragment shader");

    /* create and link program */
    GLuint program = glCreateProgram();

    glAttachShader(program, vert);
    glAttachShader(program, frag);

    GL_ERROR("attach shaders");

    /**
     * We're specifying the layout in the shaders now (this OpenGL 3.3
     * feature is thankfully supported on OSX 10.7+). So now you can
     * name your vertex attributes what you want. Be aware that the
     * engine will always choose the same order though, as detailed
     * below in the "legacy" code:
     *
     * glBindAttribLocation(program, GFX_VERTEX, "in_position");
     * glBindAttribLocation(program, GFX_NORMAL, "in_normal");
     * glBindAttribLocation(program, GFX_TEXCOORD, "in_texcoord");
     * glBindAttribLocation(program, GFX_COLOR, "in_color");
     * glBindAttribLocation(program, GFX_TANGENT, "in_tangent");
     *
     * GL_ERROR("bind attrib locations");
     */

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

    shader->loc.projectionMatrix    = glGetUniformLocation(program, "projectionMatrix");
    shader->loc.invProjectionMatrix = glGetUniformLocation(program, "invProjectMatrix");
    shader->loc.modelviewMatrix     = glGetUniformLocation(program, "modelviewMatrix");
    shader->loc.normalMatrix        = glGetUniformLocation(program, "normalMatrix");

    shader->loc.texture0            = glGetUniformLocation(program, "texture0");
    shader->loc.texture1            = glGetUniformLocation(program, "texture1");
    shader->loc.texture2            = glGetUniformLocation(program, "texture2");
    shader->loc.texture3            = glGetUniformLocation(program, "texture3");

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
