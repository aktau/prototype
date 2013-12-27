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
#include <inttypes.h>
#include <limits.h>

#include "util.h"
#include "SDL.h"

#define TEST_NAME "timer query"

static void init() {
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClearDepth(1.0f);
    glDisable(GL_DITHER);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glEnable(GL_MULTISAMPLE);
}

int main(int argc, char const *argv[]) {
    trace("setting up test: " TEST_NAME "\n");

    int width  = 800;
    int height = 600;

    SDL_Init(SDL_INIT_VIDEO);

    /* set the opengl context version, this is the latest that OSX can handle, for now... */
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 4);

    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

    SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);

    SDL_Window *window = SDL_CreateWindow(
        "timer query test",
        SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED,
        width, height,
        SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE
    );

    SDL_GLContext glcontext = SDL_GL_CreateContext(window);

    glViewport(0, 0, (GLsizei) width, (GLsizei) height);
    init();

    trace("starting test: " TEST_NAME "\n");

    for (int i = 0; i < 100000; ++i) {
        GLint64 t = 123456789;
        glGetInteger64v(GL_TIMESTAMP, &t);
        GL_ERROR("set timer");

        trace("val = %"PRId64"\n", t);
    }

    return 0;
}
