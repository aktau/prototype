/**
 * This file is part of prototype.
 *
 * (c) 2013 Nicolas Hillegeer <nicolas@hillegeer.com>
 *
 * For the full copyright and license information, please view the LICENSE
 * file that was distributed with the source code.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>

#ifdef __AVX__
#include <immintrin.h>
#define HAVE_AVX_STRING "yes"
#else
#define HAVE_AVX_STRING "no"
#endif

#ifdef __SSE__
#include <xmmintrin.h>
#define HAVE_SSE_STRING "yes"
#else
#define HAVE_SSE_STRING "no"
#endif

#define ARRAY_SIZE(x) ((sizeof(x)/sizeof(0[x])) / ((size_t)(!(sizeof(x) % sizeof(0[x])))))

#define GFX_PI                  3.14159265f
#define GFX_PI_DIV_180          GFX_PI/180.0f
#define GFX_180_DIV_PI          180.0f/GFX_PI

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include <math/math.h>

/* confuse the compiler */
volatile int the_mask = 0;
volatile int other_mask = 1;

void padstring(char *s, int to, char with, int bufsize) {
    --bufsize;

    while (--bufsize && *s++ != '\0') {
        if (!--to) return;
    }

    // printf("bufsize remaining: %d\n", bufsize);

    if (!bufsize) return;

    *(s - 1) = with;

    while (--bufsize) {
        if (!--to) return;

        // printf("adding: %c, bufsize: %d, to: %d\n", with, bufsize, to);

        *s++ = with;
    }

    *s = '\0';
}

const char *printv(vec4 vec) {
    static char buffer[256];

    const float *x = (const float*)&vec;
    snprintf(buffer, 256, "%2.3f %2.3f %2.3f %2.3f", x[0], x[1], x[2], x[3]);

    return buffer;
}

const char *printva(const float *x) {
    static char buffer[256];

    snprintf(buffer, 256, "%2.3f %2.3f %2.3f %2.3f", x[0], x[1], x[2], x[3]);

    return buffer;
}

static inline void gfxQuaFromAngleAxis(float angle, const float *restrict axis, float *restrict qua) __attribute__((always_inline));
static inline void gfxQuaFromAngleAxis(float angle, const float *restrict axis, float *restrict qua) {
    const float halfAngle = angle * 0.5f;
    const float sinA = sinf(halfAngle * GFX_PI_DIV_180);
    const float cosA = cosf(halfAngle * GFX_PI_DIV_180);

    qua[0] = axis[0]*sinA;
    qua[1] = axis[1]*sinA;
    qua[2] = axis[2]*sinA;
    qua[3] = cosA;
}

int main(int argc, char* argv[]) {
    struct timeval t1, t2;
    double elapsedTime;

    int iterations = 100000000;

    srand(time(NULL));

    float x = (float) rand()/RAND_MAX;
    float y = (float) rand()/RAND_MAX;
    float z = (float) rand()/RAND_MAX;

    volatile vec4 base = vec(1.0f, 0.0f, 0.0f, GFX_PI / 4.0f);

    printf("performing %d quat ops per function, (SSE: %s, AVX: %s)\n", iterations, HAVE_SSE_STRING, HAVE_AVX_STRING);

    {
        char name[256] = "VECTOR";
        padstring(name, 20, ' ', 256);

        vec4 bases[] = {
            vec(x, x, x, x),
            vec(y, y, y, y),
            vec(z, z, z, z)
        };
        vec4 nqua;
        vec4 accum;

        int idx = 0;

        gettimeofday(&t1, NULL);
        for (int i = 0; i < iterations; ++i) {
            idx += (idx >= 2) ? -idx : 1;

            nqua = quat_axisangle(bases[idx]);

        }
        gettimeofday(&t2, NULL);

        elapsedTime = (t2.tv_sec - t1.tv_sec) * 1000.0; /* sec to ms */
        elapsedTime += (t2.tv_usec - t1.tv_usec) / 1000.0; /* us to ms */

        printf("%s %6.1f ms [result = %s]", name, elapsedTime, printv(nqua));
        printf(" [base = %s]\n", printv(bases[idx]));
    }


    {
        char name[256] = "VECTOR-TOUGH";
        padstring(name, 20, ' ', 256);

        vec4 nqua;
        vec4 accum = vec(1.0f, 0.0f, 0.0f, GFX_PI / 4.0f);

        gettimeofday(&t1, NULL);
        for (int i = 0; i < iterations; ++i) {
            nqua = quat_axisangle(accum);
            accum = nqua;
        }
        gettimeofday(&t2, NULL);

        elapsedTime = (t2.tv_sec - t1.tv_sec) * 1000.0; /* sec to ms */
        elapsedTime += (t2.tv_usec - t1.tv_usec) / 1000.0; /* us to ms */

        printf("%s %6.1f ms [result = %s]\n", name, elapsedTime, printv(nqua));
    }

    {
        char name[256] = "VECTOR-SHUF";
        padstring(name, 20, ' ', 256);

        vec4 bases[] = {
            vec(x, x, x, x),
            vec(y, y, y, y),
            vec(z, z, z, z)
        };
        // vec4 base = vec(1.0f, 0.0f, 0.0f, GFX_PI / 4.0f);
        vec4 nqua;
        vec4 accum;

        gettimeofday(&t1, NULL);
        int idx = -1;
        for (int i = 0; i < iterations; ++i) {
            idx += (idx >= 2) ? -idx : 1;

            nqua = quat_axisangle_shuf(bases[idx]);
        }
        gettimeofday(&t2, NULL);

        elapsedTime = (t2.tv_sec - t1.tv_sec) * 1000.0; /* sec to ms */
        elapsedTime += (t2.tv_usec - t1.tv_usec) / 1000.0; /* us to ms */

        printf("%s %6.1f ms [result = %s]", name, elapsedTime, printv(nqua));
        printf(" [base = %s]\n", printv(bases[idx]));
    }


    {
        char name[256] = "VECTOR-SHUF-TOUGH";
        padstring(name, 20, ' ', 256);

        vec4 nqua;
        vec4 accum = vec(1.0f, 0.0f, 0.0f, GFX_PI / 4.0f);

        gettimeofday(&t1, NULL);
        for (int i = 0; i < iterations; ++i) {
            // nqua = quat_axisangle(vec(1.0f, 0.0f, 0.0f, GFX_PI / 4.0f));
            nqua = quat_axisangle_shuf(accum);
            accum = nqua;
        }
        gettimeofday(&t2, NULL);

        elapsedTime = (t2.tv_sec - t1.tv_sec) * 1000.0; /* sec to ms */
        elapsedTime += (t2.tv_usec - t1.tv_usec) / 1000.0; /* us to ms */

        printf("%s %6.1f ms [result = %s]\n", name, elapsedTime, printv(nqua));
    }

    {
        char name[256] = "VECTOR-CLEVER-EASY";
        padstring(name, 20, ' ', 256);

        vec4 bases[] = {
            vec(x, x, x, x),
            vec(y, y, y, y),
            vec(z, z, z, z)
        };
        // vec4 base = vec(1.0f, 0.0f, 0.0f, GFX_PI / 4.0f);
        vec4 nqua;
        vec4 accum;

        gettimeofday(&t1, NULL);
        for (int i = 0, idx = 0; i < iterations; ++i) {
            nqua = quat_axisangle_clever(base);
        }
        gettimeofday(&t2, NULL);

        elapsedTime = (t2.tv_sec - t1.tv_sec) * 1000.0; /* sec to ms */
        elapsedTime += (t2.tv_usec - t1.tv_usec) / 1000.0; /* us to ms */

        printf("%s %6.1f ms [result = %s]", name, elapsedTime, printv(nqua));
        printf(" [base = %s]\n", printv(base));
    }

    {
        char name[256] = "VECTOR-CLEVER";
        padstring(name, 20, ' ', 256);

        vec4 bases[] = {
            vec(x, x, x, x),
            vec(y, y, y, y),
            vec(z, z, z, z)
        };
        vec4 nqua;
        vec4 accum;

        gettimeofday(&t1, NULL);
        int idx = 0;
        for (int i = 0; i < iterations; ++i) {
            idx += (idx >= 2) ? -idx : 1;

            nqua = quat_axisangle_clever(bases[idx]);
        }
        gettimeofday(&t2, NULL);

        elapsedTime = (t2.tv_sec - t1.tv_sec) * 1000.0; /* sec to ms */
        elapsedTime += (t2.tv_usec - t1.tv_usec) / 1000.0; /* us to ms */

        printf("%s %6.1f ms [result = %s]", name, elapsedTime, printv(nqua));
        printf(" [base = %s]\n", printv(bases[idx]));
    }

    {
        char name[256] = "VECTOR-CLEVER-TOUGH";
        padstring(name, 20, ' ', 256);

        vec4 nqua;
        vec4 accum = vec(1.0f, 0.0f, 0.0f, GFX_PI / 4.0f);

        gettimeofday(&t1, NULL);
        for (int i = 0; i < iterations; ++i) {
            // nqua = quat_axisangle(vec(1.0f, 0.0f, 0.0f, GFX_PI / 4.0f));
            nqua = quat_axisangle_clever(accum);
            accum = nqua;
        }
        gettimeofday(&t2, NULL);

        elapsedTime = (t2.tv_sec - t1.tv_sec) * 1000.0; /* sec to ms */
        elapsedTime += (t2.tv_usec - t1.tv_usec) / 1000.0; /* us to ms */

        printf("%s %6.1f ms [result = %s]\n", name, elapsedTime, printv(nqua));
    }

    {
        char name[256] = "SCALAR";
        padstring(name, 20, ' ', 256);

        float yaxis[] = { 1.0f, 0.0f, 0.0f };
        float yqua[4];

        gettimeofday(&t1, NULL);
        for (int i = 0; i < iterations; ++i) {
            int j = i & the_mask;

            gfxQuaFromAngleAxis(45.0f, &yaxis[j], &yqua[j]);
        }
        gettimeofday(&t2, NULL);

        elapsedTime = (t2.tv_sec - t1.tv_sec) * 1000.0; /* sec to ms */
        elapsedTime += (t2.tv_usec - t1.tv_usec) / 1000.0; /* us to ms */

        printf("%s %6.1f ms [result = %s]\n", name, elapsedTime, printva(yqua));
    }

    {
        char name[256] = "SCALAR-TOUGH";
        padstring(name, 20, ' ', 256);

        float yaxis[] = { 1.0f, 0.0f, 0.0f };
        float yqua[4];
        float angle = 45.0f;

        gettimeofday(&t1, NULL);
        for (int i = 0; i < iterations; ++i) {
            int j = i & the_mask;

            gfxQuaFromAngleAxis(angle, &yaxis[j], &yqua[j]);

            memcpy(yaxis, yqua, sizeof(float) * 3);
            angle = yqua[3];
        }
        gettimeofday(&t2, NULL);

        elapsedTime = (t2.tv_sec - t1.tv_sec) * 1000.0; /* sec to ms */
        elapsedTime += (t2.tv_usec - t1.tv_usec) / 1000.0; /* us to ms */

        printf("%s %6.1f ms [result = %s]\n", name, elapsedTime, printva(yqua));
    }

    return 0;
}
