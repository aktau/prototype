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

#include "math/math.h"

#include "util.h"
#include "vec.h"

#include "SDL.h"

const char *printv(vec4 vec) {
    static char buffer[256];

    const float *x = (const float*)&vec;
    snprintf(buffer, 256, "%2.3f\t%2.3f\t%2.3f\t%2.3f", x[0], x[1], x[2], x[3]);

    return buffer;
}

const char *printva(const float *x) {
    static char buffer[256];

    snprintf(buffer, 256, "%2.3f\t%2.3f\t%2.3f\t%2.3f", x[0], x[1], x[2], x[3]);

    return buffer;
}

const char *printvi(vec4 vec) {
    static char buffer[256];

    const uint32_t *x = (const uint32_t*)&vec;
    snprintf(buffer, 256, "%8x\t%8x\t%8x\t%8x\n", x[0], x[1], x[2], x[3]);

    return buffer;
}

const char *printm(mat4 mat) {
    static char buffer[256];
    int accum = 0;

    const float *m = (const float*)&mat;

    accum += snprintf(buffer + accum, 256 - accum, "\n");
    for (int i = 0; i < 4; ++i) {
        accum += snprintf(buffer + accum, 256 - accum, "%2.3f\t%2.3f\t%2.3f\t%2.3f\n", m[i + 0], m[i + 4], m[i + 8], m[i + 12]);
    }
    accum += snprintf(buffer + accum, 256 - accum, "\n");

    return buffer;
}

const char *printm3(mat4 mat) {
    static char buffer[256];
    int accum = 0;

    const float *m = (const float*)&mat;

    accum += snprintf(buffer + accum, 256 - accum, "\n");
    for (int i = 0; i < 3; ++i) {
        accum += snprintf(buffer + accum, 256 - accum, "%2.3f\t%2.3f\t%2.3f\n", m[i + 0], m[i + 4], m[i + 8]);
    }
    accum += snprintf(buffer + accum, 256 - accum, "\n");

    return buffer;
}

static void init() {
    /* set the background black */
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    /* depth buffer setup */
    glClearDepth(1.0f);

    /* disable dithering */
    glDisable(GL_DITHER);

    /* Enables Depth Testing */
    glEnable(GL_DEPTH_TEST);

    /* The Type Of Depth Test To Do */
    glDepthFunc(GL_LEQUAL);

    /* MSAA */
    glEnable(GL_MULTISAMPLE);
}

static void gfxRender(const struct gfxModel *model, const struct gfxRenderParams *params, const struct gfxShaderProgram *program) {
    glUseProgram(program->id);
    glBindVertexArray(model->vao);

    if (model->texture[0]) {
        glActiveTexture(GL_TEXTURE0 + 0);
        glBindTexture(GL_TEXTURE_2D, model->texture[0]);
    }

    gfxSetShaderParams(program, params);

    glDrawElements(GL_TRIANGLES, model->numIndices, GL_UNSIGNED_BYTE, (GLvoid*)0);

    glBindVertexArray(0);
    glUseProgram(0);
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

        sprintf(
            title,
            "avg fps: %d, actual fps: %d, ms/f (min: %d, avg: %d, max: %d), frames: %d, mem: %zu b, lua mem: %d kb\n",
            avgfps, fps, min, avg, max, counter, zmalloc_used_memory(), wfScriptMemUsed()
        );

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

    trace("prototype/warfare engine, starting up\n");
    trace("compiler: %s\n", wfCompiler());

    trace("supported vector instructions: ");
    #ifdef __SSE__
        printf("SSE ");
    #endif
    #ifdef __SSE2__
        printf("SSE2 ");
    #endif
    #ifdef __SSE3__
        printf("SSE3 ");
    #endif
    #ifdef __SSE4__
        printf("SSE4 ");
    #endif
    #ifdef __SSE4_1__
        printf("SSE4.1 ");
    #endif
    #ifdef __SSE4_2__
        printf("SSE4.2 ");
    #endif
    #ifdef __AVX__
        printf("AVX ");
    #endif
    #ifdef __FMA4__
        printf("FMA4 ");
    #endif
    printf("\n");

    #ifdef HAVE_LUA
        trace("scripting enabled, %s\n", wfScriptVersion());
        wfScriptInit();
    #endif

    SDL_Init(SDL_INIT_VIDEO);

    /* set the opengl context version, this is the latest that OSX can handle, for now... */
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 4);

    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, doublebuf);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

    SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);

    trace("creating SDL window\n");

    SDL_Window *window = SDL_CreateWindow(
        "SDL2/OpenGL prototype",
        SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED,
        width, height,
        SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE
    );

    trace("creating OpenGL context\n");

    uint32_t t = SDL_GetTicks();

    SDL_GLContext glcontext = SDL_GL_CreateContext(window);

    trace("took %u ms to setup the OpenGL context\n", SDL_GetTicks() - t);

    printGlInfo();

    init();

    SDL_Event event;
    Uint8 done = 0;

    if (SDL_GL_SetSwapInterval(vsync) == -1) {
        trace("could not set desired vsync mode: %d", vsync);
    }

    trace("starting to render, vsync is %d\n", SDL_GL_GetSwapInterval());

    glViewport(0, 0, (GLsizei) width, (GLsizei) height);

    struct gfxShaderProgram shader;
    gfxLoadShaderFromFile(&shader, "./src/shaders/texture.vert", "./src/shaders/texture.frag");

    struct gfxShaderProgram colorShader;
    gfxLoadShaderFromFile(&colorShader, "./src/shaders/color.vert", "./src/shaders/color.frag");

    struct gfxShaderProgram guiShader;
    gfxLoadShaderFromFile(&guiShader, "./src/shaders/gui.vert", "./src/shaders/gui.frag");

    struct gfxRenderParams world = { 0 };
    gfxCreateRenderParams(&world);
    world.cull = GFX_CULL_BACK;

    struct gfxRenderParams gui = { 0 };
    gfxCreateRenderParams(&gui);
    gui.blend = GFX_BLEND_ALPHA;

    struct gfxModel quad;
    gfxQuad(&quad);

    struct gfxModel axis;
    gfxAxis(&axis);

    struct gfxModel crystal;
    gfxCrystal(&crystal);

    struct gfxModel cube;
    gfxCube(&cube);

    glActiveTexture(GL_TEXTURE0 + 0);
    GLuint texture = gfxLoadTexture("./game/img/monolith.png");
    crystal.texture[0] = texture;

    int rotate = 0;
    int reversemult = 0;
    int combined = 0;

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

                            glViewport(0, 0, (GLsizei) newWidth, (GLsizei) newHeight);

                            width  = newWidth;
                            height = newHeight;
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

                        case SDLK_r:
                            rotate = !rotate;
                            trace("rotation is now %d\n", rotate);
                            break;

                        case SDLK_c:
                            combined = (combined + 1) % 3;
                            trace("combined is now %d\n", combined);
                            break;

                        case SDLK_o:
                            reversemult = !reversemult;
                            trace("reverse is now %d\n", reversemult);
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
            }
        }

        /* clear the screen before rendering */
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        uint32_t ticks = SDL_GetTicks();
        float ms = (float) ticks * 0.001f;
        float alpha = (ticks % 5000) / 5000.0f;

        /* translation */
        mat4 transmat = midentity();
        {
            transmat.cols[3][2] = 2.0f * cosf(ms * 10.0f) - 4.0f;
        }

        /* rotation */
        mat4 rotmat;
        {
            vec4 xaxis = vunit3(vec(1.0f, 0.0f, 0.0f, 0.0f));
            xaxis[3]   = GFX_PI * (ms);
            vec4 xqua  = quat_axisangle(xaxis);

            vec4 yaxis = vunit3(vec(0.0f, 1.0f, 0.0f, 0.0f));
            yaxis[3]   = GFX_PI * 0.25f;
            vec4 yqua  = quat_axisangle(yaxis);

            vec4 neutqua = { 0.0f, 0.0f, 0.0f, 1.0f };

            vec4 finalqua;
            mat4 finalmat;

            if (reversemult) {
                finalqua = qprod(yqua, xqua);
            }
            else {
                finalqua = qprod(xqua, yqua);
            }

            if (rotate) {
                if (combined == 0) finalmat = quat_to_mat(xqua);
                else if (combined == 1) finalmat = quat_to_mat(yqua);
                else finalmat = quat_to_mat(finalqua);
            }
            else {
                finalmat = quat_to_mat(neutqua);
            }

            /* better to store, stream or to assign? */
            rotmat = finalmat;
        }

        mat4 modmat = mmmul(transmat, rotmat);
        mstoreu(world.modelviewMatrix, modmat);

        /* TODO: should factor out the changes and the re-uploading */
        /**
         * according to some, recreating the buffer is actually faster than changing the old buffer, maybe
         * true if you're having sync issues?
         * glBufferData(GL_UNIFORM_BUFFER, sizeof(struct gfxGlobalMatrices), &world.matrices, GL_STREAM_DRAW);
         */
        {
            mat4 projmat = mat_perspective_fovy(GFX_PI / 2.0f, (float) width / (float) height, 0.5f, 10.0f);
            mstoreu(world.matrices.projectionMatrix, projmat);
            glBindBuffer(GL_UNIFORM_BUFFER, world.matrixUbo);
            glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(struct gfxGlobalMatrices), &world.matrices);
            glBindBuffer(GL_UNIFORM_BUFFER, 0);

            world.timer = ms;

            /* start a batch and render */
            gfxBatch(&world);
            gfxRender(&axis, &world, &colorShader);
            gfxRender(&crystal, &world, &shader);
            gfxRender(&cube, &world, &colorShader);
        }

        {
            glBindBuffer(GL_UNIFORM_BUFFER, gui.matrixUbo);
            glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(struct gfxGlobalMatrices), &gui.matrices);
            glBindBuffer(GL_UNIFORM_BUFFER, 0);
            gui.timer = ms;

            gfxBatch(&gui);
            gfxRender(&quad, &gui, &guiShader);
        }

        SDL_GL_SwapWindow(window);

        diagFrameDone(window);
    }

    gfxDestroyModel(&crystal);
    gfxDestroyModel(&quad);
    gfxDestroyModel(&cube);
    gfxDestroyModel(&axis);
    gfxDestroyShader(&shader);
    gfxDestroyShader(&colorShader);
    gfxDestroyShader(&guiShader);

    /* TODO: destroy UBO's */

#ifdef HAVE_LUA
    wfScriptDestroy();
#endif

    SDL_GL_DeleteContext(glcontext);

    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
