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

#ifndef THREEDEE_TRANSFORMATION_H
#define THREEDEE_TRANSFORMATION_H

#include <math/vector.h>
#include <math/matrix.h>

static inline mat4 mtranslate_zero(vec4 delta) __attribute__((always_inline));
static inline mat4 mtranslate_zero(vec4 delta)
{
    vec4 one = _mm_set_ss(1.0);
    vec4 row0 = vshuffle(one, one, 0, 1, 1, 3);
    vec4 row1 = vshuffle(one, one, 1, 0, 1, 3);
    vec4 row2 = vshuffle(one, one, 1, 1, 0, 3);
    vec4 row3 = vshuffle(delta, vshuffle(delta, one, 2, 2, 0, 1), 0, 1, 0, 2);

    mat4 m = {{ row0, row1, row2, row3 }};
    return m;
}

static inline mat4 mtranslate(vec4 delta) __attribute__((always_inline));
static inline mat4 mtranslate(vec4 delta)
{
    return mtranslate_zero(vxyz(delta));
}

static inline mat4 mscale_zero(vec4 factor) __attribute__((always_inline));
static inline mat4 mscale_zero(vec4 factor)
{
    vec4 one = _mm_set_ss(1.0);
    vec4 col0 = vshuffle(factor, vzero(), 0, 3, 0, 0);
    vec4 col1 = vshuffle(factor, vzero(), 3, 1, 0, 0);
    vec4 col2 = vshuffle(vzero(), factor, 0, 0, 2, 3);
    vec4 col3 = vshuffle(one, one, 1, 1, 1, 0);
    mat4 m = {{ col0, col1, col2, col3 }};
    return m;
}

static inline mat4 mscale(vec4 factor) __attribute__((always_inline));
static inline mat4 mscale(vec4 factor)
{
    return mscale_zero(vxyz(factor));
}

#endif
