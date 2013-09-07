/**
 * This file is part of prototype.
 *
 * (c) 2013 Nicolas Hillegeer <nicolas@hillegeer.com>
 *
 * For the full copyright and license information, please view the LICENSE
 * file that was distributed with the source code.
 */

#include "util.h"

void gfxPerspectiveMatrix(float fov, float aspect, float near, float far, float *mat) {
    float top, bottom, left, right;

    top    = near * tanf(fov * GFX_PI / 360.0f);
    bottom = -top;
    left   = bottom * aspect;
    right  = top * aspect;

    memset(mat, 0x0, sizeof(float)*16);

    mat[0]  = near/right;
    mat[5]  = near/top;
    mat[10] = (-(far+near)) / (far-near);
    mat[11] = -1;
    mat[14] = (-2*far*near) / (far-near);
}

void gfxAltPerspectiveMatrix(float near, float far, float *mat) {
    float frustumScale = 1.0f;

    mat[0]  = frustumScale;
    mat[5]  = frustumScale;
    mat[10] = (far + near) / (near - far);
    mat[11] = -1.0f;
    mat[14] = (2 * far * near) / (near - far);
}


void gfxOrthoMatrix(float left, float right, float bottom, float top, float near, float far, float *matrix) {
    memset(matrix, 0x0, sizeof(float) * 16);

    matrix[0]  = 2/(right-left);
    matrix[5]  = 2/(top-bottom);
    matrix[10] = -2/(far-near);
    matrix[12] = -((right+left)/(right-left));
    matrix[13] = -((top+bottom)/(top-bottom));
    matrix[14] = -((far+near)/(far-near));
    matrix[15] = 1;
}
