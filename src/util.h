#ifndef __util_h__
#define __util_h__

#include <errno.h>

/* for debug printing with timestamping support */
#include <time.h>

/* for strerror_r */
#include <string.h>

#define GL3_PROTOTYPES
#include <OpenGL/gl3.h>

#include "SDL.h"

#ifdef DEBUG
#define DEBUG_TEST 1
#else
#define DEBUG_TEST 0
#endif

#define GL_ERROR \
    do { \
        GLenum _tmp_error = glGetError(); \
        if (_tmp_error != GL_NO_ERROR) { \
            fprintf(stderr, "error: Could not create the shaders: %s \n", wfGlErrorString(_tmp_error)); \
            exit(-1); \
        } \
    } while(0)

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


#define clean_errno() (errno == 0 ? "None" : strerror(errno))

/**
 * if cond, report err, execute msg and return with val, threadsafe
 *
 * example:
 * int retval = some_func(arg);
 * handle_error_return(retval == -1, errno, -1, "this is horrible! %s", some_string);
 */
#define ERROR_RETURN(cond, err, val, ...) \
        do {\
                if ((cond)) {\
                        fprintf(stderr, " (%d: '%s')\n", (errno), clean_errno());\
                        trace("[ERROR] " __VA_ARGS__);\
                        return (val);\
                }\
        } while (0)

#define ERROR_HANDLE(cond, err, ...) \
        do {\
                if ((cond)) {\
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

#define ARRAY_SIZE(x) ((sizeof(x)/sizeof(0[x])) / ((size_t)(!(sizeof(x) % sizeof(0[x])))))

#define TWOPI_OVER_360 0.0174533

const char* wfGlErrorString(GLenum error);

/* caller frees string when done */
char* loadfile(char *filename);

#endif
