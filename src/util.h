/**
 * This file is part of prototype.
 *
 * (c) 2013 Nicolas Hillegeer <nicolas@hillegeer.com>
 *
 * For the full copyright and license information, please view the LICENSE
 * file that was distributed with the source code.
 */

#ifndef __util_h__
#define __util_h__

#include <errno.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <math.h>
#include <inttypes.h>

#define GL3_PROTOTYPES
#ifdef __APPLE__
    #include <OpenGL/gl3.h>
    #include <OpenGL/gl3ext.h>
#else
    #include <GL/gl.h>
#endif

#include "macros.h"

#include "SDL.h"
#include "zmalloc.h"

#include "drawlist.h"
#include "gfx.h"

#ifdef DEBUG
#define DEBUG_TEST 1
#else
#define DEBUG_TEST 0
#endif

#define trace(...) \
    do {\
        if (DEBUG_TEST) {\
            /* fprintf(stderr, "%s:%d:%s(): ", __FILE__, __LINE__, __func__); */ \
            char __timestr[32];\
            time_t __timestamp;\
            struct tm *__timeinfo;\
            time(&__timestamp);\
            __timeinfo = localtime(&__timestamp);\
            strftime(__timestr, 32, "%d/%m %H:%M:%S", __timeinfo);\
            fprintf(stderr, "[%s] %s(): ", __timestr, __func__);\
            fprintf(stderr, __VA_ARGS__);\
        }\
    } while(0)

#define GL_ERROR(...) \
    if (DEBUG_TEST) { \
        for (GLenum _tmp_error = glGetError(); GL_NO_ERROR != _tmp_error; _tmp_error = glGetError()) { \
            trace("[ERROR] " __VA_ARGS__); \
            fprintf(stderr, " [OpenGL] %s \n", wfGlErrorString(_tmp_error)); \
            exit(1); \
        } \
    }

#define GL_FRAMEBUFFER_ERROR() \
    if (DEBUG_TEST) {\
        assert(wfGlFbErrorString() == NULL); \
    }

#define clean_errno() (errno == 0 ? "None" : strerror(errno))

#define BENCH_START(identifier) \
    uint64_t _bench_##identifier = SDL_GetPerformanceCounter();

/* trace("[BENCHMARK]: %" PRIu64 " ticks passed on [" STR(identifier) "]\n", _bench_##identifier); */
#define BENCH_END(identifier) \
    trace("[BENCHMARK]: %-6.2fμs [" STR(identifier) "]\n", 1000000 * (SDL_GetPerformanceCounter() - _bench_##identifier) / (float)SDL_GetPerformanceFrequency());


/**
 * if cond, report err, execute msg and return with val, threadsafe
 *
 * example:
 * int retval = some_func(arg);
 * handle_error_return(retval == -1, errno, -1, "this is horrible! %s", some_string);
 */
#define ERROR_RETURN(cond, err, val, ...) \
        do {\
                if (unlikely(cond)) {\
                        fprintf(stderr, " (%d: '%s')\n", (errno), clean_errno());\
                        trace("[ERROR] " __VA_ARGS__);\
                        return (val);\
                }\
        } while (0)

#define ERROR_EXIT(cond, err, ...) \
        do {\
                if (unlikely(cond)) {\
                        fprintf(stderr, " (%d: '%s')\n", (errno), clean_errno());\
                        trace("[ERROR] " __VA_ARGS__);\
                        exit(42);\
                }\
        } while (0)

#define ERROR_HANDLE(cond, err, ...) \
        do {\
                if (unlikely(cond)) {\
                        fprintf(stderr, " (%d: '%s')\n", (errno), clean_errno());\
                        trace("[ERROR] " __VA_ARGS__);\
                        goto error;\
                }\
        } while (0)

#ifndef MAX
#define MAX(x, y) (((x) > (y)) ? (x) : (y))
#endif

#ifndef MIN
#define MIN(x, y) (((x) < (y)) ? (x) : (y))
#endif

#define XSTR(a) STR(a)
#define STR(a) #a

#define ARRAY_SIZE(x) ((sizeof(x)/sizeof(0[x])) / ((size_t)(!(sizeof(x) % sizeof(0[x])))))

#define TWOPI_OVER_360 0.0174533

#define WF_STATIC_STRING_BUFSIZE 512

#ifdef HAVE_LUA
void wfScriptInit(void);
void wfScriptDestroy(void);
int wfScriptMemUsed(void);
const char *wfScriptVersion(void);
#endif

/* error handling */
const char *wfGlErrorString(GLenum error);
const char *wfGlFbErrorString();
void wfGlCheckLeaks(void);

/* caller frees string when done */
char *loadfile(const char *filename);

/* version.c */
const char *wfCompiler(void);

/* texture.c */
GLuint gfxLoadTexture(const char *image);
void gfxDestroyTexture(GLuint texture);

/* gfx/shader.c */
void gfxLoadShaderFromFile(struct gfxShaderProgram *shader, const char *vertfile, const char *fragfile);
void gfxLoadShader(struct gfxShaderProgram *shader, const char *vertsrc, const char *fragsrc);
void gfxDestroyShader(struct gfxShaderProgram *shader);

int gfxShaderUboLayout(const struct gfxShaderProgram *shader, struct gfxUboLayout *layout, const char *blockname);
void gfxShaderFreeUboLayout(struct gfxUboLayout *layout);
void gfxShaderPrintUboLayout(const struct gfxShaderProgram *shader, const char *blockname);

void gfxSetShaderParams(
    const struct gfxShaderProgram *shader,
    const struct gfxLayer *layer,
    const struct gfxRenderParams *params,
    const struct gfxRenderParams *prev
);

/* gfx/model.c */
void gfxDestroyModel(struct gfxModel *model);

/* gfx/renderer.c */
void gfxCreateLayer(struct gfxLayer *layer);
void gfxUploadLayer(const struct gfxLayer *layer);
void gfxDestroyLayer(struct gfxLayer *layer);
void gfxCreateRenderParams(struct gfxRenderParams *params);
void gfxDestroyRenderParams(struct gfxRenderParams *params);
void gfxBatch(const struct gfxLayer *layer);

/* gfx/perf.c */
void gfxGenQueries(struct gfxQuerySet *set);
void gfxDestroyQueries(struct gfxQuerySet *set);
void gfxBeginQuery(struct gfxQuerySet *set, GLenum target, gfx_query_t type);
void gfxEndQuery(struct gfxQuerySet *set, GLenum target);
void gfxPerfFinishFrame(struct gfxQuerySet *set);
uint32_t gfxPerfGetu32(struct gfxQuerySet *set, gfx_query_t type);
uint64_t gfxPerfGetu64(struct gfxQuerySet *set, gfx_query_t type);

/* scratch.c */
void gfxQuad(struct gfxModel *model);
void gfxCube(struct gfxModel *model);
void gfxCrystal(struct gfxModel *model);
void gfxAxis(struct gfxModel *model);
void gfxSheet(struct gfxModel *model, float width, float height, unsigned int subdiv);

#endif
