/**
 * This file is part of prototype.
 *
 * (c) 2013 Nicolas Hillegeer <nicolas@hillegeer.com>
 *
 * For the full copyright and license information, please view the LICENSE
 * file that was distributed with the source code.
 */

#ifndef __types_h__
#define __types_h__

enum GFX_VBO_TYPE {
    GFX_VBO_VERTEX = 0,
    GFX_VBO_NORMAL,
    GFX_VBO_TEXCOORD,
    GFX_VBO_COLOR,
    GFX_VBO_TANGENT,
    GFX_VBO_NUM
};

/* best to allocate sub-arrays in one fell swoop or use a really good allocator */
struct gfxModel {
    unsigned int vao;
    unsigned int vbo[GFX_VBO_NUM];
    unsigned int ibo;

    unsigned int texture[1];
};

#endif
