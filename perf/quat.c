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

    printf("performing %d quat ops per function, (SSE: %s, AVX: %s)\n", iterations, HAVE_SSE_STRING, HAVE_AVX_STRING);

    {
        const char *name = "VECTOR";

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
            nqua = quat_axisangle(bases[idx]);
            // nqua = quat_axisangle(vec(base[j], 0.0f, 0.0f, GFX_PI / 4.0f));
            // nqua = quat_axisangle(nqua);

            idx += (idx > 2) ? -idx : 1;
        }
        gettimeofday(&t2, NULL);

        elapsedTime = (t2.tv_sec - t1.tv_sec) * 1000.0; /* sec to ms */
        elapsedTime += (t2.tv_usec - t1.tv_usec) / 1000.0; /* us to ms */
        printf("%6.1f ms (%s)\n", elapsedTime, name);

        printf("wut... %s\n", printv(nqua));
    }


    {
        const char *name = "VECTOR-TOUGH";

        vec4 nqua;
        vec4 accum = vec(1.0f, 0.0f, 0.0f, GFX_PI / 4.0f);

        gettimeofday(&t1, NULL);
        for (int i = 0; i < iterations; ++i) {
            // nqua = quat_axisangle(vec(1.0f, 0.0f, 0.0f, GFX_PI / 4.0f));
            nqua = quat_axisangle(accum);
            accum = nqua;
        }
        gettimeofday(&t2, NULL);

        elapsedTime = (t2.tv_sec - t1.tv_sec) * 1000.0; /* sec to ms */
        elapsedTime += (t2.tv_usec - t1.tv_usec) / 1000.0; /* us to ms */
        printf("%6.1f ms (%s)\n", elapsedTime, name);

        printf("wut... %s\n", printv(nqua));
    }


    {
        const char *name = "SCALAR";

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
        printf("%6.1f ms (%s)\n", elapsedTime, name);

        printf("wut... %s\n", printva(yqua));
    }

    {
        const char *name = "SCALAR-TOUGH";

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
        printf("%6.1f ms (%s)\n", elapsedTime, name);

        printf("wut... %s\n", printva(yqua));
    }

    return 0;
}
