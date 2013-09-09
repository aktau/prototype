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

#ifndef THREEDEE_TYPES_H
#define THREEDEE_TYPES_H

typedef float vec4 __attribute__((vector_size(16)));
typedef float scalar;

struct mat4_t
{
    vec4 cols[4];
} __attribute__((aligned(16)));

typedef struct mat4_t mat4;

#endif
