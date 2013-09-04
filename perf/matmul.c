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

typedef union {
    float m[4][4];
    __m128 row[4];
} Mat44;

static void __attribute__ ((noinline)) mmulDummy(const float *restrict m1, const float *restrict m2, float *restrict m3) {

}

static void __attribute__ ((noinline)) mmulDummyU(const Mat44 *restrict m1, const Mat44 *restrict m2, Mat44 *restrict m3) {

}

static void __attribute__ ((noinline)) matmul4x4(const float* m1, const float* m2, float* m3) {
    m3[0]  = m1[0]*m2[0] + m1[1]*m2[4] + m1[2]*m2[8] + m1[3]*m2[12];
    m3[1]  = m1[0]*m2[1] + m1[1]*m2[5] + m1[2]*m2[9] + m1[3]*m2[13];
    m3[2]  = m1[0]*m2[2] + m1[1]*m2[6] + m1[2]*m2[10] + m1[3]*m2[14];
    m3[3]  = m1[0]*m2[3] + m1[1]*m2[7] + m1[2]*m2[11] + m1[3]*m2[15];
    m3[4]  = m1[4]*m2[0] + m1[5]*m2[4] + m1[6]*m2[8] + m1[7]*m2[12];
    m3[5]  = m1[4]*m2[1] + m1[5]*m2[5] + m1[6]*m2[9] + m1[7]*m2[13];
    m3[6]  = m1[4]*m2[2] + m1[5]*m2[6] + m1[6]*m2[10] + m1[7]*m2[14];
    m3[7]  = m1[4]*m2[3] + m1[5]*m2[7] + m1[6]*m2[11] + m1[7]*m2[15];
    m3[8]  = m1[8]*m2[0] + m1[9]*m2[4] + m1[10]*m2[8] + m1[11]*m2[12];
    m3[9]  = m1[8]*m2[1] + m1[9]*m2[5] + m1[10]*m2[9] + m1[11]*m2[13];
    m3[10] = m1[8]*m2[2] + m1[9]*m2[6] + m1[10]*m2[10] + m1[11]*m2[14];
    m3[11] = m1[8]*m2[3] + m1[9]*m2[7] + m1[10]*m2[11] + m1[11]*m2[15];
    m3[12] = m1[12]*m2[0] + m1[13]*m2[4] + m1[14]*m2[8] + m1[15]*m2[12];
    m3[13] = m1[12]*m2[1] + m1[13]*m2[5] + m1[14]*m2[9] + m1[15]*m2[13];
    m3[14] = m1[12]*m2[2] + m1[13]*m2[6] + m1[14]*m2[10] + m1[15]*m2[14];
    m3[15] = m1[12]*m2[3] + m1[13]*m2[7] + m1[14]*m2[11] + m1[15]*m2[15];
}

static void __attribute__ ((noinline)) matmul4x4res(const float *restrict m1, const float *restrict m2, float *restrict m3) {
    m3[0]  = m1[0]*m2[0] + m1[1]*m2[4] + m1[2]*m2[8] + m1[3]*m2[12];
    m3[1]  = m1[0]*m2[1] + m1[1]*m2[5] + m1[2]*m2[9] + m1[3]*m2[13];
    m3[2]  = m1[0]*m2[2] + m1[1]*m2[6] + m1[2]*m2[10] + m1[3]*m2[14];
    m3[3]  = m1[0]*m2[3] + m1[1]*m2[7] + m1[2]*m2[11] + m1[3]*m2[15];
    m3[4]  = m1[4]*m2[0] + m1[5]*m2[4] + m1[6]*m2[8] + m1[7]*m2[12];
    m3[5]  = m1[4]*m2[1] + m1[5]*m2[5] + m1[6]*m2[9] + m1[7]*m2[13];
    m3[6]  = m1[4]*m2[2] + m1[5]*m2[6] + m1[6]*m2[10] + m1[7]*m2[14];
    m3[7]  = m1[4]*m2[3] + m1[5]*m2[7] + m1[6]*m2[11] + m1[7]*m2[15];
    m3[8]  = m1[8]*m2[0] + m1[9]*m2[4] + m1[10]*m2[8] + m1[11]*m2[12];
    m3[9]  = m1[8]*m2[1] + m1[9]*m2[5] + m1[10]*m2[9] + m1[11]*m2[13];
    m3[10] = m1[8]*m2[2] + m1[9]*m2[6] + m1[10]*m2[10] + m1[11]*m2[14];
    m3[11] = m1[8]*m2[3] + m1[9]*m2[7] + m1[10]*m2[11] + m1[11]*m2[15];
    m3[12] = m1[12]*m2[0] + m1[13]*m2[4] + m1[14]*m2[8] + m1[15]*m2[12];
    m3[13] = m1[12]*m2[1] + m1[13]*m2[5] + m1[14]*m2[9] + m1[15]*m2[13];
    m3[14] = m1[12]*m2[2] + m1[13]*m2[6] + m1[14]*m2[10] + m1[15]*m2[14];
    m3[15] = m1[12]*m2[3] + m1[13]*m2[7] + m1[14]*m2[11] + m1[15]*m2[15];
}

static void __attribute__ ((noinline)) matmul4x4resstream(const float *restrict m1, const float *restrict m2, float *restrict m3) {
    const float l0 = m1[0];
    const float l1 = m1[1];
    const float l2 = m1[2];
    const float l3 = m1[3];

    const float l4 = m1[4];
    const float l5 = m1[5];
    const float l6 = m1[6];
    const float l7 = m1[7];

    const float l8 = m1[8];
    const float l9 = m1[9];
    const float l10 = m1[10];
    const float l11 = m1[11];

    const float l12 = m1[12];
    const float l13 = m1[13];
    const float l14 = m1[14];
    const float l15 = m1[15];

    const float r0 = m2[0];
    const float r1 = m2[1];
    const float r2 = m2[2];
    const float r3 = m2[3];

    const float r4 = m2[4];
    const float r5 = m2[5];
    const float r6 = m2[6];
    const float r7 = m2[7];

    const float r8 = m2[8];
    const float r9 = m2[9];
    const float r10 = m2[10];
    const float r11 = m2[11];

    const float r12 = m2[12];
    const float r13 = m2[13];
    const float r14 = m2[14];
    const float r15 = m2[15];

    const float res0 = l0 * r0 + l1 * r4 + l2 * r8 + l3 * r12;
    const float res1 = l0 * r1 + l1 * r5 + l2 * r9 + l3 * r13;
    const float res2 = l0 * r2 + l1 * r6 + l2 * r10 + l3 * r14;
    const float res3 = l0 * r3 + l1 * r7 + l2 * r11 + l3 * r15;

    const float res4 = l4 * r0 + l5 * r4 + l6 * r8 + l7 * r12;
    const float res5 = l4 * r1 + l5 * r5 + l6 * r9 + l7 * r13;
    const float res6 = l4 * r2 + l5 * r6 + l6 * r10 + l7 * r14;
    const float res7 = l4 * r3 + l5 * r7 + l6 * r11 + l7 * r15;

    const float res8  = l8 * r0 + l9 * r4 + l10 * r8 + l11 * r12;
    const float res9  = l8 * r1 + l9 * r5 + l10 * r9 + l11 * r13;
    const float res10 = l8 * r2 + l9 * r6 + l10 * r10 + l11 * r14;
    const float res11 = l8 * r3 + l9 * r7 + l10 * r11 + l11 * r15;

    const float res12 = l12 * r0 + l13 * r4 + l14 * r8 + l15 * r12;
    const float res13 = l12 * r1 + l13 * r5 + l14 * r9 + l15 * r13;
    const float res14 = l12 * r2 + l13 * r6 + l14 * r10 + l15 * r14;
    const float res15 = l12 * r3 + l13 * r7 + l14 * r11 + l15 * r15;

    m3[0]  = res0;
    m3[1]  = res1;
    m3[2]  = res2;
    m3[3]  = res3;

    m3[4]  = res4;
    m3[5]  = res5;
    m3[6]  = res6;
    m3[7]  = res7;

    m3[8]  = res8;
    m3[9]  = res9;
    m3[10] = res10;
    m3[11] = res11;

    m3[12] = res12;
    m3[13] = res13;
    m3[14] = res14;
    m3[15] = res15;
}

/**
 * source: http://fhtr.blogspot.de/2010/02/4x4-float-matrix-multiplication-using.html
 */
static void __attribute__ ((noinline)) matmul4x4sse(const float *restrict a, const float *restrict b, float *restrict r) {
  __m128 a_line;
  __m128 b_line;
  __m128 r_line;

  for (int i = 0; i < 16; i += 4) {
    // unroll the first step of the loop to avoid having to initialize r_line to zero
    a_line = _mm_load_ps(b);         // a_line = vec4(column(a, 0)) -- shouldn't that be vec4(row(a, 0)) ?
    b_line = _mm_set1_ps(a[i]);      // b_line = vec4(b[i][0])
    r_line = _mm_mul_ps(a_line, b_line); // r_line = a_line * b_line

    for (int j = 1; j < 4; ++j) {
      a_line = _mm_load_ps(&b[j*4]); // a_line = vec4(column(a, j))
      b_line = _mm_set1_ps(a[i+j]);  // b_line = vec4(b[i][j])
                                     // r_line += a_line * b_line
      r_line = _mm_add_ps(_mm_mul_ps(a_line, b_line), r_line);
    }

    _mm_store_ps(&r[i], r_line);     // r[i] = r_line
  }
}

/**
 * source: http://fhtr.blogspot.de/2010/02/4x4-float-matrix-multiplication-using.html
 */
static void __attribute__ ((noinline)) matmul4x4sseunr(const float *restrict a, const float *restrict b, float *restrict r) {
  __m128 a_line;
  __m128 b_line;
  __m128 r_line;

  for (int i = 0; i < 16; i += 4) {
    // unroll the first step of the loop to avoid having to initialize r_line to zero
    a_line = _mm_load_ps(&b[0]);         // a_line = vec4(column(a, 0)) -- shouldn't that be vec4(row(a, 0)) ?
    b_line = _mm_set1_ps(a[i]);      // b_line = vec4(b[i][0])
    r_line = _mm_mul_ps(a_line, b_line); // r_line = a_line * b_line

    a_line = _mm_load_ps(&b[4]);
    b_line = _mm_set1_ps(a[i+1]);
    r_line = _mm_add_ps(_mm_mul_ps(a_line, b_line), r_line);

    a_line = _mm_load_ps(&b[8]);
    b_line = _mm_set1_ps(a[i+2]);
    r_line = _mm_add_ps(_mm_mul_ps(a_line, b_line), r_line);

    a_line = _mm_load_ps(&b[12]);
    b_line = _mm_set1_ps(a[i+3]);
    r_line = _mm_add_ps(_mm_mul_ps(a_line, b_line), r_line);

    _mm_store_ps(&r[i], r_line);     // r[i] = r_line
  }
}

/**
 * source: http://fhtr.blogspot.de/2010/02/4x4-float-matrix-multiplication-using.html
 */
static void __attribute__ ((noinline)) matmul4x4sseunr2(const float *restrict a, const float *restrict b, float *restrict r) {
    __m128 a_line;
    __m128 b_line;
    __m128 r_line;

    a_line = _mm_load_ps(&b[0]);
    b_line = _mm_set1_ps(a[0]);
    r_line = _mm_mul_ps(a_line, b_line);

    a_line = _mm_load_ps(&b[4]);
    b_line = _mm_set1_ps(a[1]);
    r_line = _mm_add_ps(_mm_mul_ps(a_line, b_line), r_line);

    a_line = _mm_load_ps(&b[8]);
    b_line = _mm_set1_ps(a[2]);
    r_line = _mm_add_ps(_mm_mul_ps(a_line, b_line), r_line);

    a_line = _mm_load_ps(&b[12]);
    b_line = _mm_set1_ps(a[3]);
    r_line = _mm_add_ps(_mm_mul_ps(a_line, b_line), r_line);

    _mm_store_ps(&r[0], r_line);

    a_line = _mm_load_ps(&b[0]);
    b_line = _mm_set1_ps(a[4]);
    r_line = _mm_mul_ps(a_line, b_line);

    a_line = _mm_load_ps(&b[4]);
    b_line = _mm_set1_ps(a[5]);
    r_line = _mm_add_ps(_mm_mul_ps(a_line, b_line), r_line);

    a_line = _mm_load_ps(&b[8]);
    b_line = _mm_set1_ps(a[6]);
    r_line = _mm_add_ps(_mm_mul_ps(a_line, b_line), r_line);

    a_line = _mm_load_ps(&b[12]);
    b_line = _mm_set1_ps(a[7]);
    r_line = _mm_add_ps(_mm_mul_ps(a_line, b_line), r_line);

    _mm_store_ps(&r[4], r_line);

    a_line = _mm_load_ps(&b[0]);
    b_line = _mm_set1_ps(a[8]);
    r_line = _mm_mul_ps(a_line, b_line);

    a_line = _mm_load_ps(&b[4]);
    b_line = _mm_set1_ps(a[9]);
    r_line = _mm_add_ps(_mm_mul_ps(a_line, b_line), r_line);

    a_line = _mm_load_ps(&b[8]);
    b_line = _mm_set1_ps(a[10]);
    r_line = _mm_add_ps(_mm_mul_ps(a_line, b_line), r_line);

    a_line = _mm_load_ps(&b[12]);
    b_line = _mm_set1_ps(a[11]);
    r_line = _mm_add_ps(_mm_mul_ps(a_line, b_line), r_line);

    _mm_store_ps(&r[8], r_line);

    a_line = _mm_load_ps(&b[0]);
    b_line = _mm_set1_ps(a[12]);
    r_line = _mm_mul_ps(a_line, b_line);

    a_line = _mm_load_ps(&b[4]);
    b_line = _mm_set1_ps(a[13]);
    r_line = _mm_add_ps(_mm_mul_ps(a_line, b_line), r_line);

    a_line = _mm_load_ps(&b[8]);
    b_line = _mm_set1_ps(a[14]);
    r_line = _mm_add_ps(_mm_mul_ps(a_line, b_line), r_line);

    a_line = _mm_load_ps(&b[12]);
    b_line = _mm_set1_ps(a[15]);
    r_line = _mm_add_ps(_mm_mul_ps(a_line, b_line), r_line);

    _mm_store_ps(&r[12], r_line);
}

/**
 * source: http://fhtr.blogspot.de/2010/02/4x4-float-matrix-multiplication-using.html
 */
static void __attribute__ ((noinline)) matmul4x4sseunr2compact(const float *restrict a, const float *restrict b, float *restrict r) {
    __m128 r_line;

    r_line = _mm_mul_ps(_mm_load_ps(&b[0]), _mm_set1_ps(a[0]));
    r_line = _mm_add_ps(_mm_mul_ps(_mm_load_ps(&b[4]), _mm_set1_ps(a[1])), r_line);
    r_line = _mm_add_ps(_mm_mul_ps(_mm_load_ps(&b[8]), _mm_set1_ps(a[2])), r_line);
    r_line = _mm_add_ps(_mm_mul_ps(_mm_load_ps(&b[12]), _mm_set1_ps(a[3])), r_line);

    _mm_store_ps(&r[0], r_line);

    r_line = _mm_mul_ps(_mm_load_ps(&b[0]), _mm_set1_ps(a[4]));
    r_line = _mm_add_ps(_mm_mul_ps(_mm_load_ps(&b[4]), _mm_set1_ps(a[5])), r_line);
    r_line = _mm_add_ps(_mm_mul_ps(_mm_load_ps(&b[8]), _mm_set1_ps(a[6])), r_line);
    r_line = _mm_add_ps(_mm_mul_ps(_mm_load_ps(&b[12]), _mm_set1_ps(a[7])), r_line);

    _mm_store_ps(&r[4], r_line);

    r_line = _mm_mul_ps(_mm_load_ps(&b[0]), _mm_set1_ps(a[8]));
    r_line = _mm_add_ps(_mm_mul_ps(_mm_load_ps(&b[4]), _mm_set1_ps(a[9])), r_line);
    r_line = _mm_add_ps(_mm_mul_ps(_mm_load_ps(&b[8]), _mm_set1_ps(a[10])), r_line);
    r_line = _mm_add_ps(_mm_mul_ps(_mm_load_ps(&b[12]), _mm_set1_ps(a[11])), r_line);

    _mm_store_ps(&r[8], r_line);

    r_line = _mm_mul_ps(_mm_load_ps(&b[0]), _mm_set1_ps(a[12]));
    r_line = _mm_add_ps(_mm_mul_ps(_mm_load_ps(&b[4]), _mm_set1_ps(a[13])), r_line);
    r_line = _mm_add_ps(_mm_mul_ps(_mm_load_ps(&b[8]), _mm_set1_ps(a[14])), r_line);
    r_line = _mm_add_ps(_mm_mul_ps(_mm_load_ps(&b[12]), _mm_set1_ps(a[15])), r_line);

    _mm_store_ps(&r[12], r_line);
}

/**
 * https://gist.github.com/rygorous/4172889
 * linear combination:
 * a[0] * B.row[0] + a[1] * B.row[1] + a[2] * B.row[2] + a[3] * B.row[3]
 */
static inline __m128 lincomb_SSE(const __m128 a, const float *B) {
    __m128 result;

    result = _mm_mul_ps(_mm_shuffle_ps(a, a, 0x00), _mm_load_ps(&B[0]));
    result = _mm_add_ps(result, _mm_mul_ps(_mm_shuffle_ps(a, a, 0x55), _mm_load_ps(&B[4])));
    result = _mm_add_ps(result, _mm_mul_ps(_mm_shuffle_ps(a, a, 0xaa), _mm_load_ps(&B[8])));
    result = _mm_add_ps(result, _mm_mul_ps(_mm_shuffle_ps(a, a, 0xff), _mm_load_ps(&B[12])));

    return result;
}

// this is the right approach for SSE ... SSE4.2
void __attribute__ ((noinline)) matmul4x4sse42(const float *restrict A, const float *restrict B, float *restrict out) {
    __m128 out0x = lincomb_SSE(_mm_load_ps(&A[0]), B);
    __m128 out1x = lincomb_SSE(_mm_load_ps(&A[4]), B);
    __m128 out2x = lincomb_SSE(_mm_load_ps(&A[8]), B);
    __m128 out3x = lincomb_SSE(_mm_load_ps(&A[12]), B);

    _mm_store_ps(&out[0], out0x);
    _mm_store_ps(&out[4], out1x);
    _mm_store_ps(&out[8], out2x);
    _mm_store_ps(&out[12], out3x);
}

void __attribute__ ((noinline)) matmul4x4sse42unr(const float *restrict A, const float *restrict B, float *restrict out) {
    __m128 out0x;
    __m128 out1x;
    __m128 out2x;
    __m128 out3x;

    {
        const __m128 a = _mm_load_ps(&A[0]);
        out0x = _mm_mul_ps(_mm_shuffle_ps(a, a, 0x00), _mm_load_ps(&B[0]));
        out0x = _mm_add_ps(out0x, _mm_mul_ps(_mm_shuffle_ps(a, a, 0x55), _mm_load_ps(&B[4])));
        out0x = _mm_add_ps(out0x, _mm_mul_ps(_mm_shuffle_ps(a, a, 0xaa), _mm_load_ps(&B[8])));
        out0x = _mm_add_ps(out0x, _mm_mul_ps(_mm_shuffle_ps(a, a, 0xff), _mm_load_ps(&B[12])));
    }

    {
        const __m128 a = _mm_load_ps(&A[4]);
        out1x = _mm_mul_ps(_mm_shuffle_ps(a, a, 0x00), _mm_load_ps(&B[0]));
        out1x = _mm_add_ps(out1x, _mm_mul_ps(_mm_shuffle_ps(a, a, 0x55), _mm_load_ps(&B[4])));
        out1x = _mm_add_ps(out1x, _mm_mul_ps(_mm_shuffle_ps(a, a, 0xaa), _mm_load_ps(&B[8])));
        out1x = _mm_add_ps(out1x, _mm_mul_ps(_mm_shuffle_ps(a, a, 0xff), _mm_load_ps(&B[12])));
    }

    {
        const __m128 a = _mm_load_ps(&A[8]);
        out2x = _mm_mul_ps(_mm_shuffle_ps(a, a, 0x00), _mm_load_ps(&B[0]));
        out2x = _mm_add_ps(out2x, _mm_mul_ps(_mm_shuffle_ps(a, a, 0x55), _mm_load_ps(&B[4])));
        out2x = _mm_add_ps(out2x, _mm_mul_ps(_mm_shuffle_ps(a, a, 0xaa), _mm_load_ps(&B[8])));
        out2x = _mm_add_ps(out2x, _mm_mul_ps(_mm_shuffle_ps(a, a, 0xff), _mm_load_ps(&B[12])));
    }

    {
        const __m128 a = _mm_load_ps(&A[12]);
        out3x = _mm_mul_ps(_mm_shuffle_ps(a, a, 0x00), _mm_load_ps(&B[0]));
        out3x = _mm_add_ps(out3x, _mm_mul_ps(_mm_shuffle_ps(a, a, 0x55), _mm_load_ps(&B[4])));
        out3x = _mm_add_ps(out3x, _mm_mul_ps(_mm_shuffle_ps(a, a, 0xaa), _mm_load_ps(&B[8])));
        out3x = _mm_add_ps(out3x, _mm_mul_ps(_mm_shuffle_ps(a, a, 0xff), _mm_load_ps(&B[12])));
    }

    _mm_store_ps(&out[0], out0x);
    _mm_store_ps(&out[4], out1x);
    _mm_store_ps(&out[8], out2x);
    _mm_store_ps(&out[12], out3x);
}

void __attribute__ ((noinline)) matmul4x4sse42rew(const float *restrict A, const float *restrict B, float *restrict out) {
    __m128 temp;
    __m128 a;

    a = _mm_load_ps(&A[0]);
    temp = _mm_mul_ps(_mm_shuffle_ps(a, a, 0x00), _mm_load_ps(&B[0]));
    temp = _mm_add_ps(temp, _mm_mul_ps(_mm_shuffle_ps(a, a, 0x55), _mm_load_ps(&B[4])));
    temp = _mm_add_ps(temp, _mm_mul_ps(_mm_shuffle_ps(a, a, 0xaa), _mm_load_ps(&B[8])));
    temp = _mm_add_ps(temp, _mm_mul_ps(_mm_shuffle_ps(a, a, 0xff), _mm_load_ps(&B[12])));
    _mm_store_ps(&out[0], temp);

    a = _mm_load_ps(&A[4]);
    temp = _mm_mul_ps(_mm_shuffle_ps(a, a, 0x00), _mm_load_ps(&B[0]));
    temp = _mm_add_ps(temp, _mm_mul_ps(_mm_shuffle_ps(a, a, 0x55), _mm_load_ps(&B[4])));
    temp = _mm_add_ps(temp, _mm_mul_ps(_mm_shuffle_ps(a, a, 0xaa), _mm_load_ps(&B[8])));
    temp = _mm_add_ps(temp, _mm_mul_ps(_mm_shuffle_ps(a, a, 0xff), _mm_load_ps(&B[12])));
    _mm_store_ps(&out[4], temp);

    a = _mm_load_ps(&A[8]);
    temp = _mm_mul_ps(_mm_shuffle_ps(a, a, 0x00), _mm_load_ps(&B[0]));
    temp = _mm_add_ps(temp, _mm_mul_ps(_mm_shuffle_ps(a, a, 0x55), _mm_load_ps(&B[4])));
    temp = _mm_add_ps(temp, _mm_mul_ps(_mm_shuffle_ps(a, a, 0xaa), _mm_load_ps(&B[8])));
    temp = _mm_add_ps(temp, _mm_mul_ps(_mm_shuffle_ps(a, a, 0xff), _mm_load_ps(&B[12])));
    _mm_store_ps(&out[8], temp);

    a = _mm_load_ps(&A[12]);
    temp = _mm_mul_ps(_mm_shuffle_ps(a, a, 0x00), _mm_load_ps(&B[0]));
    temp = _mm_add_ps(temp, _mm_mul_ps(_mm_shuffle_ps(a, a, 0x55), _mm_load_ps(&B[4])));
    temp = _mm_add_ps(temp, _mm_mul_ps(_mm_shuffle_ps(a, a, 0xaa), _mm_load_ps(&B[8])));
    temp = _mm_add_ps(temp, _mm_mul_ps(_mm_shuffle_ps(a, a, 0xff), _mm_load_ps(&B[12])));
    _mm_store_ps(&out[12], temp);
}

#ifdef __AVX__

/**
 * https://gist.github.com/rygorous/4172889
 */

// dual linear combination using AVX instructions on YMM regs
static inline __m256 twolincomb_AVX_8_pointer(__m256 A01, const float *restrict B) {
    __m256 result;

    // const __m128 B0 = _mm_load_ps(&B[0]);
    // const __m128 B1 = _mm_load_ps(&B[4]);
    // const __m128 B2 = _mm_load_ps(&B[8]);
    // const __m128 B3 = _mm_load_ps(&B[12]);

    // result = _mm256_mul_ps(_mm256_shuffle_ps(A01, A01, 0x00), _mm256_broadcast_ps(&B0));
    // result = _mm256_add_ps(result, _mm256_mul_ps(_mm256_shuffle_ps(A01, A01, 0x55), _mm256_broadcast_ps(&B1)));
    // result = _mm256_add_ps(result, _mm256_mul_ps(_mm256_shuffle_ps(A01, A01, 0xaa), _mm256_broadcast_ps(&B2)));
    // result = _mm256_add_ps(result, _mm256_mul_ps(_mm256_shuffle_ps(A01, A01, 0xff), _mm256_broadcast_ps(&B3)));

    result = _mm256_mul_ps(_mm256_shuffle_ps(A01, A01, 0x00), _mm256_broadcast_ss(&B[0]));
    result = _mm256_add_ps(result, _mm256_mul_ps(_mm256_shuffle_ps(A01, A01, 0x55), _mm256_broadcast_ss(&B[4])));
    result = _mm256_add_ps(result, _mm256_mul_ps(_mm256_shuffle_ps(A01, A01, 0xaa), _mm256_broadcast_ss(&B[8])));
    result = _mm256_add_ps(result, _mm256_mul_ps(_mm256_shuffle_ps(A01, A01, 0xff), _mm256_broadcast_ss(&B[12])));

    return result;
}

// this should be noticeably faster with actual 256-bit wide vector units (Intel);
// not sure about double-pumped 128-bit (AMD), would need to check.
void __attribute__ ((noinline)) matmul4x4avx8x(const float *restrict A, const float *restrict B, float *restrict out) {
    _mm256_zeroupper();
    __m256 A01 = _mm256_loadu_ps(&A[0]);
    __m256 A23 = _mm256_loadu_ps(&A[8]);

    __m256 out01x = twolincomb_AVX_8_pointer(A01, B);
    __m256 out23x = twolincomb_AVX_8_pointer(A23, B);

    _mm256_storeu_ps(&out[0], out01x);
    _mm256_storeu_ps(&out[8], out23x);
}

// dual linear combination using AVX instructions on YMM regs
static inline __m256 twolincomb_AVX_8(__m256 A01, const Mat44 *restrict B) {
    __m256 result;
    result = _mm256_mul_ps(_mm256_shuffle_ps(A01, A01, 0x00), _mm256_broadcast_ps(&B->row[0]));
    result = _mm256_add_ps(result, _mm256_mul_ps(_mm256_shuffle_ps(A01, A01, 0x55), _mm256_broadcast_ps(&B->row[1])));
    result = _mm256_add_ps(result, _mm256_mul_ps(_mm256_shuffle_ps(A01, A01, 0xaa), _mm256_broadcast_ps(&B->row[2])));
    result = _mm256_add_ps(result, _mm256_mul_ps(_mm256_shuffle_ps(A01, A01, 0xff), _mm256_broadcast_ps(&B->row[3])));
    return result;
}

// this should be noticeably faster with actual 256-bit wide vector units (Intel);
// not sure about double-pumped 128-bit (AMD), would need to check.
void __attribute__ ((noinline)) matmul4x4avx8xunion(const Mat44 *restrict A, const Mat44 *restrict B, Mat44 *restrict out) {
    _mm256_zeroupper();
    __m256 A01 = _mm256_loadu_ps(&A->m[0][0]);
    __m256 A23 = _mm256_loadu_ps(&A->m[2][0]);

    __m256 out01x = twolincomb_AVX_8(A01, B);
    __m256 out23x = twolincomb_AVX_8(A23, B);

    _mm256_storeu_ps(&out->m[0][0], out01x);
    _mm256_storeu_ps(&out->m[2][0], out23x);
}

#endif

void printMatrix(const float *m) {
    for (int row=0; row < 4; ++row) {
        for (int column = 0; column < 4; ++column) {
            printf("%6.1f     ", m[column + row * 4]);
        }
        printf("\n");
     }
}

void printMat44(const Mat44 *m) {
    for (int row=0; row < 4; ++row) {
        for (int column = 0; column < 4; ++column) {
            printf("%6.1f     ", m->m[row][column]);
        }
        printf("\n");
     }
}

/**
 * in-place version
 * source: http://drrobsjournal.blogspot.de/2012/10/fast-simd-4x4-matrix-multiplication.html
 */

/**
 * TODO:
 *
 * check the assembly, a compiler might cheat because it knows what the
 * matrices are and perhaps that they will be constant...
 *
 * check the output to see if they calculate the same thing
 *
 * SSE3, horizontal add: http://fastcpp.blogspot.de/2011/03/matrix-vector-multiplication-using-sse3.html
 */
typedef void (*mmul_t)(const float *restrict, const float *restrict, float *restrict);
typedef void (*mmul_union_t)(const Mat44 *restrict, const Mat44 *restrict, Mat44 *restrict);

struct benchSetup {
    char *name;
    mmul_t mmulFunction;
};

void inline benchmarkUnion(const char *name, mmul_union_t mmulFunction) {
    struct timeval t1, t2;
    double elapsedTime;

    int iterations = 100000000;

    Mat44 one = { .m = {
        { 0, 1, 2, 3 },
        { 4, 5, 6, 7 },
        { 8, 9, 10, 11 },
        { 12, 13, 14, 15 }
    } };

    Mat44 two = { .m = {
        { 4, 3, 2, 1 },
        { 7, 6, 5, 4 },
        { 11, 10, 9, 8 },
        { 15, 14, 13, 12 }
    } };

    Mat44 three;

    gettimeofday(&t1, NULL);
    for (int i = 0; i < iterations; ++i) {
        mmulFunction(&one, &two, &three);
    }
    gettimeofday(&t2, NULL);

    elapsedTime = (t2.tv_sec - t1.tv_sec) * 1000.0; /* sec to ms */
    elapsedTime += (t2.tv_usec - t1.tv_usec) / 1000.0; /* us to ms */
    printf("%6.1f ms (%s)\n", elapsedTime, name);

    // printMat44(&three);
    // memcpy(three, baseTwo, sizeof(float) * 16);
}

void inline benchmark(const char *name, mmul_t mmulFunction) {
    struct timeval t1, t2;
    double elapsedTime;

    int iterations = 100000000;

    float one[16] = {
        0, 1, 2, 3,
        4, 5, 6, 7,
        8, 9, 10, 11,
        12, 13, 14, 15
    };

    float two[16] = {
        4, 3, 2, 1,
        7, 6, 5, 4,
        11, 10, 9, 8,
        15, 14, 13, 12
    };

    float three[16];

    gettimeofday(&t1, NULL);
    for (int i = 0; i < iterations; ++i) {
        mmulFunction(one, two, three);
    }
    gettimeofday(&t2, NULL);

    elapsedTime = (t2.tv_sec - t1.tv_sec) * 1000.0; /* sec to ms */
    elapsedTime += (t2.tv_usec - t1.tv_usec) / 1000.0; /* us to ms */
    printf("%6.1f ms (%s)\n", elapsedTime, name);

    // printMatrix(three);
    // memcpy(three, baseTwo, sizeof(float) * 16);
}

int main(int argc, char* argv[]) {
    struct timeval t1, t2;
    double elapsedTime;

    int iterations = 100000000;

    float baseOne[16] = {
        0, 1, 2, 3,
        4, 5, 6, 7,
        8, 9, 10, 11,
        12, 13, 14, 15
    };

    float baseTwo[16] = {
        4, 3, 2, 1,
        7, 6, 5, 4,
        11, 10, 9, 8,
        15, 14, 13, 12
    };

    float one[16];
    float two[16];
    float three[16];

    memcpy(one, baseOne, sizeof(float) * 16);
    memcpy(two, baseTwo, sizeof(float) * 16);
    memcpy(three, baseTwo, sizeof(float) * 16);

    printf("performing %d matrix multiplications per function, (SSE: %s, AVX: %s)\n", iterations, HAVE_SSE_STRING, HAVE_AVX_STRING);

    struct benchSetup benches[] = {
        { "naive", matmul4x4 },
        { "restrict", matmul4x4res },
        { "restrict, stream", matmul4x4resstream },
#ifdef __AVX__
        { "avx", matmul4x4avx8x },
#else
        { "avx (dummy)", mmulDummy },
#endif
        { "restrict, sse", matmul4x4sse },
        { "restrict, sse, unrolled", matmul4x4sseunr },
        { "restrict, sse, max unrolled", matmul4x4sseunr2 },
        { "restrict, sse, max unrolled, compact", matmul4x4sseunr2compact },
        { "restrict, sse 4.2, unrolled", matmul4x4sse42 },
        { "restrict, sse 4.2, max unrolled", matmul4x4sse42unr },
        { "restrict, sse 4.2, reworked", matmul4x4sse42rew }
    };

    static const struct {
        const char *name;
        mmul_union_t mmulFunction;
    } union_benches[] = {
#ifdef __AVX__
        { "union avx", matmul4x4avx8xunion }
#else
        { "union avx (dummy)", mmulDummyU }
#endif
    };

    for (int i = 0; i < ARRAY_SIZE(union_benches); ++i) {
        benchmarkUnion(union_benches[i].name, union_benches[i].mmulFunction);
    }

    for (int i = 0; i < ARRAY_SIZE(benches); ++i) {
        benchmark(benches[i].name, benches[i].mmulFunction);
    }
    printf("\n\n");

    for (int i = ARRAY_SIZE(union_benches) - 1; i >= 0; --i) {
        benchmarkUnion(union_benches[i].name, union_benches[i].mmulFunction);
    }
    printf("\n");

    for (int i = ARRAY_SIZE(benches) - 1; i >= 0; --i) {
        benchmark(benches[i].name, benches[i].mmulFunction);
    }
    printf("\n");

    return 0;
}
