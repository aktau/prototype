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

#define GL3_PROTOTYPES
#include <OpenGL/gl3.h>

#include "SDL.h"

// #include "SDL_opengl.h"

#ifndef MAX
#define MAX(x, y) (((x) > (y)) ? (x) : (y))
#endif

#ifndef MIN
#define MIN(x, y) (((x) < (y)) ? (x) : (y))
#endif

#define ARRAY_SIZE(x) ((sizeof(x)/sizeof(0[x])) / ((size_t)(!(sizeof(x) % sizeof(0[x])))))

#define TWOPI_OVER_360 0.0174533

/**
 * TODO: switch to basic shaders
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
    // glShadeModel( GL_SMOOTH );

    /* Set the background black */
    glClearColor( 0.0f, 0.0f, 0.0f, 1.0f );

    /* Depth buffer setup */
    glClearDepth( 1.0f );

    /* Enables Depth Testing */
    glEnable(GL_DEPTH_TEST);

    /* The Type Of Depth Test To Do */
    glDepthFunc(GL_LEQUAL);

    /* Really Nice Perspective Calculations */
    // glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
}

static void render() {
    // ... can be used alongside SDL2.
    // static float x = 0.0, y = 30.0;

    /* clear screen */
    glClearColor(0,0,0,1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // glRotatef(10.0,0.0,0.0,1.0);

    // Note that the glBegin() ... glEnd() OpenGL style used below is actually
    // obsolete, but it will do for example purposes. For more information, see
    // SDL_GL_GetProcAddress() or find an OpenGL extension loading library.
    // glBegin(GL_TRIANGLES);
    //     glColor3f(1.0,0.0,0.0);
    //     glVertex2f(x, y + 90.0);

    //     glColor3f(0.0,1.0,0.0);
    //     glVertex2f(x + 90.0, y - 90.0);

    //     glColor3f(0.0,0.0,1.0);
    //     glVertex2f(x - 90.0, y - 90.0);
    // glEnd();
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
            printf("could not retrieve %s information, aborting\n", str[i]);
            exit(-1);
        }
        else {
            printf("%s: %s\n", str[i], info);
        }
    }

    GLint major;
    GLint minor;

    glGetIntegerv(GL_MAJOR_VERSION, &major);
    glGetIntegerv(GL_MINOR_VERSION, &minor);

    printf("context version double check: %d.%d\n", major, minor);
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

        sprintf(title, "avg fps: %d, actual fps: %d, ms per frame (min: %d, avg: %d, max: %d), frames processed: %d\n", avgfps, fps, min, avg, max, counter);

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
    int vsync     = 1;
    int doublebuf = 1;

    int width  = 1280;
    int height = 720;

    SDL_Init(SDL_INIT_VIDEO);

    /* set the opengl context version, this is the latest that OSX can handle, for now... */
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);

    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, doublebuf);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

    SDL_Window *window = SDL_CreateWindow(
        "SDL2/OpenGL prototype",
        SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED,
        width, height,
        SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE
    );

    SDL_GLContext glcontext = SDL_GL_CreateContext(window);

    printGlInfo();

    init();
    setupTransform(width, height);

    SDL_Event event;
    Uint8 done = 0;

    if (SDL_GL_SetSwapInterval(vsync) == -1) {
        printf("could not set desired vsync mode: %d", vsync);
    }

    printf("starting to render, vsync is %d\n", SDL_GL_GetSwapInterval());

    while (!done) {
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_KEYDOWN:
                    break;
                case SDL_KEYUP:
                    if (event.key.keysym.sym == SDLK_ESCAPE)
                        done = 1;
                    break;
                case SDL_QUIT:
                    done = 1;
                    break;
            }
        }

        render();

        SDL_GL_SwapWindow(window);
        // SDL_Delay(10);

        diagFrameDone(window);
    }

    SDL_GL_DeleteContext(glcontext);

    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
