/**
 * Copyright (c) 2013 Riku Salminen
 *
 * This software is provided 'as-is', without any express or implied
 * warranty. In no event will the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 *    1. The origin of this software must not be misrepresented; you must not
 *       claim that you wrote the original software. If you use this software
 *       in a product, an acknowledgment in the product documentation would be
 *       appreciated but is not required.
 *    2. Altered source versions must be plainly marked as such, and must not be
 *       misrepresented as being the original software.
 *    3. This notice may not be removed or altered from any source
 *       distribution.
 */

#ifndef THREEDEE_SINCOS_H
#define THREEDEE_SINCOS_H

#include <math.h>

#include <math/vector.h>
#include <math/sse_mathfun.h>

static inline void vsincos_scalar(vec4 x, vec4 * restrict s, vec4 * restrict c) __attribute__((always_inline));
static inline void vsincos_scalar(vec4 x, vec4 * restrict s, vec4 * restrict c)
{
    float *xs = (float*)&x;
    *s = vec(sinf(xs[0]), sinf(xs[1]), sinf(xs[2]), sinf(xs[3]));
    *c = vec(cosf(xs[0]), cosf(xs[1]), cosf(xs[2]), cosf(xs[3]));
}

static inline void vsincos(vec4 x, vec4 * restrict s, vec4 * restrict c) __attribute__((always_inline));
static inline void vsincos(vec4 x, vec4 * restrict s, vec4 * restrict c) { sincos_ps(x, s, c); }

static inline vec4 vsin(vec4 x) __attribute__((always_inline));
static inline vec4 vsin(vec4 x) { return sin_ps(x); }

#endif
