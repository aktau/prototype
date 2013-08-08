/**
 * This file is part of prototype.
 *
 * (c) 2013 Nicolas Hillegeer <nicolas@hillegeer.com>
 *
 * For the full copyright and license information, please view the LICENSE
 * file that was distributed with the source code.
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdint.h>
#include <limits.h>

/**
 * this is mac only, probably gl/gl3.h for linux and something else for win.
 * For win we will likely also need glew or another extension loader.
 */
#define GL3_PROTOTYPES
#include <OpenGL/gl3.h>

#include "SDL.h"

#include "util.h"
#include "vec.h"

// #include "SDL_opengl.h"

/**
 * TODO: switch to basic shaders
 * TODO: cleanup shaders fully (unuse program + delete)
 * TODO: print GL errors until no more errors: http://www.lighthouse3d.com/cg-topics/code-samples/opengl-3-3-glsl-1-5-sample/
 */

// static void perspective(GLdouble fovy, GLdouble aspect, GLdouble near, GLdouble far) {
//     GLdouble half_height = near * tan( fovy * 0.5 * TWOPI_OVER_360 );
//     GLdouble half_width = half_height * aspect;

//     glFrustum(-half_width, half_width, -half_height, half_height, near, far);
// }

static void setupTransform(int width, int height) {
    // int ortho = 1;

    // GLfloat ratio = (GLfloat) width / (GLfloat) (height ? height : 1);

    /* setup viewport */
    glViewport(0, 0, (GLsizei) width, (GLsizei) height);

    /* Now, regular OpenGL functions ... */
    // if (ortho) {
    //     glMatrixMode(GL_PROJECTION);
    //     glLoadIdentity();
    //     glOrtho(-320, 320, 240, -240, 0, 1);
    // }
    // else {
    //     glMatrixMode(GL_PROJECTION);
    //     glLoadIdentity();
    //     perspective(45.0f, ratio, 0.1f, 100.0f);
    // }

    // glMatrixMode(GL_MODELVIEW);
    // glLoadIdentity();
}

static void init() {
    /* Enable smooth shading */
    // glShadeModel(GL_SMOOTH);

    /* set the background black */
    glClearColor( 0.0f, 0.0f, 0.0f, 1.0f );

    /* depth buffer setup */
    glClearDepth( 1.0f );

    /* disable dithering */
    glDisable(GL_DITHER);

    /* Enables Depth Testing */
    // glEnable(GL_DEPTH_TEST);

    /* The Type Of Depth Test To Do */
    // glDepthFunc(GL_LEQUAL);

    /* Really Nice Perspective Calculations */
    // glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
}

static void genTriangle(GLuint *vao, GLuint *vbo, GLuint *cbo, GLuint *ibo) {
    GLfloat vertices[] = {
        0.0f, 0.0f, 0.0f, 1.0f,
        /* top */
        -0.2f, 0.8f, 0.0f, 1.0f,
        0.2f, 0.8f, 0.0f, 1.0f,
        0.0f, 0.8f, 0.0f, 1.0f,
        0.0f, 1.0f, 0.0f, 1.0f,
        /* bottom */
        -0.2f, -0.8f, 0.0f, 1.0f,
        0.2f, -0.8f, 0.0f, 1.0f,
        0.0f, -0.8f, 0.0f, 1.0f,
        0.0f, -1.0f, 0.0f, 1.0f,
        /* left */
        -0.8f, -0.2f, 0.0f, 1.0f,
        -0.8f, 0.2f, 0.0f, 1.0f,
        -0.8f, 0.0f, 0.0f, 1.0f,
        -1.0f, 0.0f, 0.0f, 1.0f,
        /* right */
        0.8f, -0.2f, 0.0f, 1.0f,
        0.8f, 0.2f, 0.0f, 1.0f,
        0.8f, 0.0f, 0.0f, 1.0f,
        1.0f, 0.0f, 0.0f, 1.0f
    };

    GLfloat colors[] = {
        1.0f, 1.0f, 1.0f, 1.0f,
        0.0f, 1.0f, 0.0f, 1.0f,
        0.0f, 0.0f, 1.0f, 1.0f,
        0.0f, 1.0f, 1.0f, 1.0f,
        1.0f, 0.0f, 0.0f, 1.0f,
        0.0f, 0.0f, 1.0f, 1.0f,
        0.0f, 1.0f, 0.0f, 1.0f,
        0.0f, 1.0f, 1.0f, 1.0f,
        1.0f, 0.0f, 0.0f, 1.0f,
        0.0f, 1.0f, 0.0f, 1.0f,
        0.0f, 0.0f, 1.0f, 1.0f,
        0.0f, 1.0f, 1.0f, 1.0f,
        1.0f, 0.0f, 0.0f, 1.0f,
        0.0f, 0.0f, 1.0f, 1.0f,
        0.0f, 1.0f, 0.0f, 1.0f,
        0.0f, 1.0f, 1.0f, 1.0f,
        1.0f, 0.0f, 0.0f, 1.0f
    };

    GLubyte indices[] = {
        /* top */
        0, 1, 3,
        0, 3, 2,
        3, 1, 4,
        3, 4, 2,
        /* bottom */
        0, 5, 7,
        0, 7, 6,
        7, 5, 8,
        7, 8, 6,
        /* left */
        0, 9, 11,
        0, 11, 10,
        11, 9, 12,
        11, 12, 10,
        /* right */
        0, 13, 15,
        0, 15, 14,
        15, 13, 16,
        15, 16, 14
    };

    // GLfloat vertices[] = {
    //     -0.8f, -0.8f, 0.0f, 1.0f,
    //      0.0f,  0.8f, 0.0f, 1.0f,
    //      0.8f, -0.8f, 0.0f, 1.0f
    // };

    // GLfloat colors[] = {
    //     1.0f, 0.0f, 0.0f, 1.0f,
    //     0.0f, 1.0f, 0.0f, 1.0f,
    //     0.0f, 0.0f, 1.0f, 1.0f
    // };

    glGenVertexArrays(1, vao);
    glBindVertexArray(*vao);

    GL_ERROR("create VAO");

    glGenBuffers(1, vbo);
    glBindBuffer(GL_ARRAY_BUFFER, *vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);

    GL_ERROR("create VBO of vertex data");

    glGenBuffers(1, cbo);
    glBindBuffer(GL_ARRAY_BUFFER, *cbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(colors), colors, GL_STATIC_DRAW);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(1);

    GL_ERROR("create VBO of colors");

    glGenBuffers(1, ibo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, *ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    GL_ERROR("create VBO of colors");
}

void destroyTriangle(GLuint *vao, GLuint *vbo, GLuint *cbo, GLuint *ibo) {
    GLenum error = glGetError();

    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    glDeleteBuffers(1, cbo);
    glDeleteBuffers(1, vbo);
    glDeleteBuffers(1, ibo);

    glBindVertexArray(0);
    glDeleteVertexArrays(1, vao);

    GL_ERROR("delete buffer objects");
}

void setupShaders(GLuint *vtshader, GLuint *fgshader, GLuint *program) {
    GLchar *vtShaderSource = (GLchar *) loadfile("./src/shaders/color.vert");
    GLchar *fgShaderSource = (GLchar *) loadfile("./src/shaders/color.frag");

    trace("vertex shader: \n%s\n", vtShaderSource);
    trace("fragment shader: \n%s\n", fgShaderSource);

    *vtshader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(*vtshader, 1, (const GLchar **) &vtShaderSource, NULL);
    glCompileShader(*vtshader);

    GL_SHADER_ERROR(*vtshader);
    GL_ERROR("create and compile vertex shader");

    *fgshader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(*fgshader, 1, (const GLchar **) &fgShaderSource, NULL);
    glCompileShader(*fgshader);

    GL_SHADER_ERROR(*fgshader);
    GL_ERROR("create and compile fragment shader");

    *program = glCreateProgram();

    glAttachShader(*program, *vtshader);
    glAttachShader(*program, *fgshader);

    GL_ERROR("attach shaders");

    glBindAttribLocation(*program, 0, "in_position");
    glBindAttribLocation(*program, 1, "in_color");

    glLinkProgram(*program);

    GL_PROGRAM_ERROR(*program);
    GL_ERROR("link shader program");

    glUseProgram(*program);

    GL_ERROR("use shader program");

    /* cleanup */
    glDetachShader(*program, *vtshader);
    glDetachShader(*program, *fgshader);

    glDeleteShader(*vtshader);
    glDeleteShader(*fgshader);

    zfree(vtShaderSource);
    zfree(fgShaderSource);
}

void destroyShaders(GLuint *vtshader, GLuint *fgshader, GLuint *program) {
    glUseProgram(0);
    glDeleteProgram(*program);

    GL_ERROR("delete shader program");
}

static void render() {
    /* clear screen */
    glClearColor(0,0,0,1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    /* glDrawArrays(GL_TRIANGLES, 0, 3); */
    glDrawElements(GL_TRIANGLES, 48, GL_UNSIGNED_BYTE, (GLvoid*)0);
}

static void printGlInfo() {
    const char *str[] = {
        "version",
        "renderer",
        "vendor",
        "shading_language_version"
    };

    const unsigned int constant[] = {
        GL_VERSION,
        GL_RENDERER,
        GL_VENDOR,
        GL_SHADING_LANGUAGE_VERSION
    };

    for (int i = 0; i < (int) ARRAY_SIZE(constant); ++i) {
        const unsigned char *info = glGetString(constant[i]);

        if (info == NULL) {
            trace("could not retrieve %s information, aborting\n", str[i]);

            exit(-1);
        }
        else {
            trace("%s: %s\n", str[i], info);
        }
    }

    GLint major;
    GLint minor;

    glGetIntegerv(GL_MAJOR_VERSION, &major);
    glGetIntegerv(GL_MINOR_VERSION, &minor);

    trace("context version double check: %d.%d\n", major, minor);
}

static void diagFrameDone(SDL_Window *window) {
    char title[1024];

    /* processing time per frame */
    static int counter = 0;

    static int min = 0, max = 0, avg = 0;
    static int elapsed = 0, totalElapsed = 0;
    static int last = 0;

    int current = (int) SDL_GetTicks();
    elapsed = (current - last);
    totalElapsed += elapsed;

    ++counter;

    min = MIN(min, elapsed);
    max = MAX(max, elapsed);
    avg += (elapsed - avg) / counter;

    /* let's average over (more or less) one second */
    if (totalElapsed >= 1000) {
        int avgfps = (int) (1000 / (float) avg);
        int fps = (int) (1000 * (float) counter / (float) totalElapsed);

        sprintf(title, "avg fps: %d, actual fps: %d, ms/f (min: %d, avg: %d, max: %d), frames processed: %d, memory: %zu\n", avgfps, fps, min, avg, max, counter, zmalloc_used_memory());

        SDL_SetWindowTitle(window, title);

        counter      = 0;
        totalElapsed = 0;

        min = INT32_MAX;
        max = 0;
        avg = 0;
    }

    last = current;
}

int main(int argc, char* argv[]) {
    int vsync     = 0;
    int doublebuf = 1;

    /* 16:9 => 704x440 */
    int width  = 800;
    int height = 600;

    trace("Prototype/warfare engine, initializing SDL\n");

    SDL_Init(SDL_INIT_VIDEO);

    /* set the opengl context version, this is the latest that OSX can handle, for now... */
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);

    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, doublebuf);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

    trace("Creating SDL window\n");

    SDL_Window *window = SDL_CreateWindow(
        "SDL2/OpenGL prototype",
        SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED,
        width, height,
        SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE
    );

    trace("Creating OpenGL context\n");

    Uint32 t = SDL_GetTicks();

    SDL_GLContext glcontext = SDL_GL_CreateContext(window);

    trace("Took %u ms to setup the OpenGL context\n", SDL_GetTicks() - t);

    printGlInfo();

    init();

#ifdef HAVE_LUA
    wfScriptInit();
#endif

    SDL_Event event;
    Uint8 done = 0;

    if (SDL_GL_SetSwapInterval(vsync) == -1) {
        trace("could not set desired vsync mode: %d", vsync);
    }

    trace("starting to render, vsync is %d\n", SDL_GL_GetSwapInterval());

    setupTransform(width, height);

    GLuint vtShader, fgShader, program;
    setupShaders(&vtShader, &fgShader, &program);

    GLuint vao, vbo, cbo, ibo;
    genTriangle(&vao, &vbo, &cbo, &ibo);

    while (!done) {
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_WINDOWEVENT:
                    switch (event.window.event) {
                        case SDL_WINDOWEVENT_RESIZED:
                        {
                            int newWidth  = event.window.data1;
                            int newHeight = event.window.data2;

                            trace(
                                "Window %d resized to %dx%d\n",
                                event.window.windowID,
                                newWidth, newHeight
                            );

                            setupTransform(newWidth, newHeight);
                        }
                        break;
                    }
                    break;

                case SDL_KEYDOWN:
                    break;

                case SDL_KEYUP:
                    switch (event.key.keysym.sym) {
                        case SDLK_ESCAPE:
                            done = 1;
                        break;

                        case SDLK_v:
                            vsync = !vsync;

                            if (SDL_GL_SetSwapInterval(vsync) == -1) {
                                trace("could not set desired vsync mode: %s\n", (vsync ? "on" : "off"));
                            }
                            else {
                                trace("turned vsync %s\n", (vsync ? "on" : "off"));
                            }
                        break;

                        case SDLK_d:
                            doublebuf = !doublebuf;

                            if (SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, doublebuf) == -1) {
                                trace("could not set desired doublebuf mode: %d\n", doublebuf);
                            }
                            else {
                                trace("turned doublebuf %s\n", (doublebuf ? "on" : "off"));
                            }
                        break;
                    }
                    break;

                case SDL_QUIT:
                    done = 1;
                    break;

                // default:
                //     trace("unkown even type received: %d\n", event.type);
            }
        }

        render();

        SDL_GL_SwapWindow(window);

        diagFrameDone(window);
    }

    destroyTriangle(&vao, &vbo, &cbo, &ibo);
    destroyShaders(&vtShader, &fgShader, &program);

    SDL_GL_DeleteContext(glcontext);

    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
