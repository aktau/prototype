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

#include <math/vector.h>
#include <math/sse_mathfun.h>

/**
 * float *xs = (float*)&x;
 * s = vec(sin(xs[0]), sin(xs[1]), sin(xs[2]), sin(xs[3]));
 * c = vec(cos(xs[0]), cos(xs[1]), cos(xs[2]), cos(xs[3]));
 */
static inline void vsincos(vec4 x, vec4 * restrict s, vec4 * restrict c) {
    sincos_ps(x, s, c);
}

#endif
