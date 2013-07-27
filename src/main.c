#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

#include "SDL.h"
#include "SDL_opengl.h"

#define TWOPI_OVER_360 0.0174533

/**
 * TODO: switch to core profile, banish immediate mode
 * TODO: switch to basic shaders
 */

static void perspective(GLdouble fovy, GLdouble aspect, GLdouble near, GLdouble far) {
    GLdouble half_height = near * tan( fovy * 0.5 * TWOPI_OVER_360 );
    GLdouble half_width = half_height * aspect;

    glFrustum(-half_width, half_width, -half_height, half_height, near, far);
}

static void setupTransform(Uint32 width, Uint32 height) {
    int ortho = 1;

    GLfloat ratio = (GLfloat) width / (GLfloat) (height ? height : 1);

    /* setup viewport */
    glViewport(0, 0, (GLsizei) width, (GLsizei) height);

    /* Now, regular OpenGL functions ... */
    if (ortho) {
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        glOrtho(-320, 320, 240, -240, 0, 1);
    }
    else {
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        perspective(45.0f, ratio, 0.1f, 100.0f);
    }

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

static void init() {
    /* Enable smooth shading */
    glShadeModel( GL_SMOOTH );

    /* Set the background black */
    glClearColor( 0.0f, 0.0f, 0.0f, 1.0f );

    /* Depth buffer setup */
    glClearDepth( 1.0f );

    /* Enables Depth Testing */
    glEnable(GL_DEPTH_TEST);

    /* The Type Of Depth Test To Do */
    glDepthFunc(GL_LEQUAL);

    /* Really Nice Perspective Calculations */
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
}

static void render() {
    // ... can be used alongside SDL2.
    static float x = 0.0, y = 30.0;

    /* clear screen */
    // glClearColor(0,0,0,1);
    glClear(GL_COLOR_BUFFER_BIT); /* | GL_DEPTH_BUFFER_BIT */

    glRotatef(10.0,0.0,0.0,1.0);

    // Note that the glBegin() ... glEnd() OpenGL style used below is actually
    // obsolete, but it will do for example purposes. For more information, see
    // SDL_GL_GetProcAddress() or find an OpenGL extension loading library.
    glBegin(GL_TRIANGLES); // drawing a multicolored triangle
        glColor3f(1.0,0.0,0.0);
        glVertex2f(x, y + 90.0);

        glColor3f(0.0,1.0,0.0);
        glVertex2f(x + 90.0, y - 90.0);

        glColor3f(0.0,0.0,1.0);
        glVertex2f(x - 90.0, y - 90.0);
    glEnd();
}

int main(int argc, char* argv[]) {
    SDL_Init(SDL_INIT_VIDEO);

    /* set the opengl context version, this is the latest that OSX can handle, for now... */
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);

    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

    int vsync = 1;

    Uint32 width = 800;
    Uint32 height = 600;

    SDL_Window *window = SDL_CreateWindow(
        "SDL2/OpenGL prototype",
        SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED,
        width, height,
        SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE
    );

    SDL_GLContext glcontext = SDL_GL_CreateContext(window);

    init();
    setupTransform(width, height);

    SDL_Event event;
    Uint8 done = 0;

    Uint32 ticks = SDL_GetTicks();

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

        Uint32 nticks = SDL_GetTicks();

        printf("wow, one whole loop, took %u ms to render!\n", nticks - ticks);

        ticks = nticks;
    }

    SDL_GL_DeleteContext(glcontext);

    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
