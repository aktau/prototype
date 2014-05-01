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

#include "math/math.h"

#include "util.h"

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

const char *printm(const mat4 mat) {
    static char buffer[256];
    size_t accum = 0;

    accum += (size_t) snprintf(buffer + accum, 256 - accum, "\n");
    for (int i = 0; i < 4; ++i) {
        accum += (size_t) snprintf(buffer + accum, 256 - accum, "%2.3f\t%2.3f\t%2.3f\t%2.3f\n", mat.cols[0][i], mat.cols[1][i], mat.cols[2][i], mat.cols[3][i]);
    }
    accum += (size_t) snprintf(buffer + accum, 256 - accum, "\n");

    return buffer;
}

const char *printm3(mat4 mat) {
    static char buffer[256];
    size_t accum = 0;

    accum += (size_t) snprintf(buffer + accum, 256 - accum, "\n");
    for (int i = 0; i < 3; ++i) {
        accum += (size_t) snprintf(buffer + accum, 256 - accum, "%2.3f\t%2.3f\t%2.3f\n", mat.cols[0][i], mat.cols[1][i], mat.cols[2][i]);
    }
    accum += (size_t) snprintf(buffer + accum, 256 - accum, "\n");

    return buffer;
}

/* call this function after initializing all relevant uniform buffer objects
 * and after every resize. */
static void resize(struct gfxRenderer *renderer, int width, int height) {
    glViewport(0, 0, (GLsizei) width, (GLsizei) height);

    struct gfxLayer **layers = renderer->layers;
    struct gfxLayer *layer = NULL;
    for (layer = *layers; layer; layer = *(++layers)) {
        /* set the projection matrix for the layer */
        switch (layer->projection) {
            case GFX_PERSPECTIVE:
                layer->uniforms.projectionMatrix = mat_perspective_fovy(GFX_PI / 2.0f, (float) width / (float) height, 0.5f, 10.0f);
                break;
            case GFX_ORTHO:
                layer->uniforms.projectionMatrix = mat_ortho(0.0f, (float) width, 0.0f, (float) height, 0.0f, 1.0f);
                break;
        }

        /* after changing the uniforms we have to re-upload */
        gfxUploadLayer(layer);
    }

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

static void fillRendererInfo(struct gfxRenderer *renderer) {
    glGetIntegerv(GL_MAX_UNIFORM_BLOCK_SIZE, &renderer->uboMaxBlockSize);
    GL_ERROR("request max uniform block size");

    glGetIntegerv(GL_MAX_UNIFORM_BUFFER_BINDINGS, &renderer->uboMaxBindings);
    GL_ERROR("request max uniform buffer bindings");

    glGetIntegerv(GL_UNIFORM_BUFFER_OFFSET_ALIGNMENT, &renderer->uboOffsetAlign);
    GL_ERROR("request uniform buffer offset alignment");
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
    #ifdef __SSSE3__
        printf("SSSE3 ");
    #endif
    #ifdef __SSE4__
        printf("SSE4 ");
    #endif
    #ifdef __SSE4A__
        printf("SSE4a ");
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
    #ifdef __AVX2__
        printf("AVX2 ");
    #endif
    #ifdef __FMA__
        printf("FMA ");
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
    gfxDrawlistDebug();

    init();

    SDL_Event event;
    Uint8 done = 0;

    if (SDL_GL_SetSwapInterval(vsync) == -1) {
        trace("could not set desired vsync mode: %d", vsync);
    }

    trace("starting to render, vsync is %d\n", SDL_GL_GetSwapInterval());

    /* shaders */
    struct gfxShaderProgram shader;
    gfxLoadShaderFromFile(&shader, "./src/shaders/texture.vert", "./src/shaders/texture.frag");
    gfxShaderPrintUboLayout(&shader, "StaticMatrices");

    struct gfxShaderProgram colorShader;
    gfxLoadShaderFromFile(&colorShader, "./src/shaders/color.vert", "./src/shaders/color.frag");

    struct gfxShaderProgram waveShader;
    gfxLoadShaderFromFile(&waveShader, "./src/shaders/wave.vert", "./src/shaders/wave.frag");

    struct gfxShaderProgram guiShader;
    gfxLoadShaderFromFile(&guiShader, "./src/shaders/gui.vert", "./src/shaders/gui.frag");

    /* render modes */
    struct gfxRenderParams world = { 0 };
    gfxCreateRenderParams(&world);
    world.cull = GFX_CULL_BACK;

    struct gfxRenderParams nocull = { 0 };
    gfxCreateRenderParams(&nocull);

    struct gfxRenderParams gui = { 0 };
    gfxCreateRenderParams(&gui);
    gui.blend = GFX_BLEND_ALPHA;

    /* layers */
    struct gfxLayer sceneLayer = {0};
    gfxCreateLayer(&sceneLayer);

    struct gfxLayer guiLayer = {0};
    gfxCreateLayer(&guiLayer);
    guiLayer.projection = GFX_ORTHO;

    /* models */
    unsigned int modelId = 1;

    struct gfxModel quad;
    gfxQuad(&quad);
    quad.id = modelId++;

    struct gfxModel axis;
    gfxAxis(&axis);
    axis.id = modelId++;

    struct gfxModel crystal;
    gfxCrystal(&crystal);
    crystal.id = modelId++;

    struct gfxModel cube;
    gfxCube(&cube);
    cube.id = modelId++;

    /* always picking the unit square is better, the shader kind of relies on it */
    struct gfxModel sheet;
    gfxSheet(&sheet, 1.0f, 1.0f, 12);
    sheet.id = modelId++;

    glActiveTexture(GL_TEXTURE0 + 0);
    GLuint texture = gfxLoadTexture("./game/img/monolith.png");
    crystal.texture[0] = texture;

    int rotate      = 0;
    int reversemult = 0;
    int combined    = 0;
    int wireframe   = 0;

    struct gfxLayer *layers[] = { &sceneLayer, &guiLayer, NULL };

    struct gfxRenderer rend = {
        .layers = layers,
    };
    fillRendererInfo(&rend);
    trace("render context info:\n");
    trace("    - max uniform block size = %d bytes\n", rend.uboMaxBlockSize);
    trace("    - max UBO binding points = %d\n", rend.uboMaxBindings);
    trace("    - UBO offset align       = %d\n", rend.uboOffsetAlign);

    resize(&rend, width, height);

    /* initial drawlist generation */
    gfxDrawlistClear();

    struct gfxDrawOperation axisd = {
        .model   = &axis,
        .params  = &world,
        .program = &colorShader,
        .layer   = &sceneLayer,
    };
    gfxGenRenderKey(&axisd);

    struct gfxDrawOperation sheetd = {
        .model   = &sheet,
        .params  = &nocull,
        .program = &waveShader,
        .layer   = &sceneLayer,
    };
    gfxGenRenderKey(&sheetd);

    struct gfxDrawOperation crystald = {
        .model   = &crystal,
        .params  = &world,
        .program = &shader,
        .layer   = &sceneLayer,
    };
    gfxGenRenderKey(&crystald);

    struct gfxDrawOperation cubed = {
        .model   = &cube,
        .params  = &world,
        .program = &colorShader,
        .layer   = &sceneLayer,
    };
    gfxGenRenderKey(&cubed);

    struct gfxDrawOperation guid = {
        .model   = &quad,
        .params  = &gui,
        .program = &guiShader,
        .layer   = &guiLayer,
    };
    gfxGenRenderKey(&guid);
    guid.key.gen.layer = LAYER_2;

    gfxDrawlistAdd(&axisd);
    gfxDrawlistAdd(&sheetd);
    gfxDrawlistAdd(&crystald);
    gfxDrawlistAdd(&cubed);
    gfxDrawlistAdd(&guid);

    struct gfxQuerySet queries = {0};
    gfxGenQueries(&queries);

    while (!done) {
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_WINDOWEVENT:
                    switch (event.window.event) {
                        case SDL_WINDOWEVENT_RESIZED:
                        {
                            width  = event.window.data1;
                            height = event.window.data2;

                            trace(
                                "Window %d resized to %dx%d\n",
                                event.window.windowID,
                                width, height
                            );

                            resize(&rend, width, height);
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

                        case SDLK_w:
                            wireframe = !wireframe;

                            glPolygonMode(GL_FRONT_AND_BACK, wireframe ? GL_LINE : GL_FILL);
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
        // float alpha = (float) (ticks % 5000) / 5000.0f;

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

            rotmat = finalmat;
        }

        mat4 modmat = mmmul(transmat, rotmat);
        world.modelviewMatrix = modmat;

        gfxBeginQuery(&queries, GL_TIME_ELAPSED, GFX_TIMER_RENDER);

        /**
         * TODO: should factor out the changes and the re-uploading
         *
         * according to some, recreating the buffer is actually faster than changing the old buffer, maybe
         * true if you're having sync issues?
         * glBufferData(GL_UNIFORM_BUFFER, sizeof(struct gfxGlobalMatrices), &world.matrices, GL_STREAM_DRAW);
         */
        sceneLayer.uniforms.timer = ms;
        guiLayer.uniforms.timer   = ms;
        gfxUploadLayer(&sceneLayer);
        gfxUploadLayer(&guiLayer);

        gfxBeginQuery(&queries, GL_PRIMITIVES_GENERATED, GFX_PRIMITIVES_GENERATED);
        gfxDrawlistRender();
        gfxEndQuery(&queries, GL_PRIMITIVES_GENERATED);

        gfxEndQuery(&queries, GL_TIME_ELAPSED);

        uint32_t beforeSwap = SDL_GetTicks();
        SDL_GL_SwapWindow(window);
        uint32_t elapsedSwap = SDL_GetTicks() - beforeSwap;

        uint32_t rendertime = gfxPerfGetu32(&queries, GFX_TIMER_RENDER);
        uint32_t nprims = gfxPerfGetu32(&queries, GFX_PRIMITIVES_GENERATED);

        /* TODO: print out when we have text rendering */
        /* printf("render objects: %u prims, %f ms, swap buffers: %f\n", */
        /*         nprims, (double) rendertime / 1000000.0, (double) elapsedSwap); */

        /* swap the query set */
        gfxPerfFinishFrame(&queries);

        diagFrameDone(window);
    }

    gfxDestroyModel(&crystal);
    gfxDestroyModel(&quad);
    gfxDestroyModel(&cube);
    gfxDestroyModel(&axis);
    gfxDestroyModel(&sheet);

    gfxDestroyShader(&shader);
    gfxDestroyShader(&colorShader);
    gfxDestroyShader(&guiShader);
    gfxDestroyShader(&waveShader);

    gfxDestroyRenderParams(&world);
    gfxDestroyRenderParams(&gui);
    gfxDestroyRenderParams(&nocull);

    gfxDestroyLayer(&sceneLayer);
    gfxDestroyLayer(&guiLayer);

    gfxDestroyTexture(texture);

    gfxDestroyQueries(&queries);

#ifdef HAVE_LUA
    wfScriptDestroy();
#endif

    wfGlCheckLeaks();

    SDL_GL_DeleteContext(glcontext);

    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
