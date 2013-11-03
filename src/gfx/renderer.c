/**
 * This file is part of prototype.
 *
 * (c) 2013 Nicolas Hillegeer <nicolas@hillegeer.com>
 *
 * For the full copyright and license information, please view the LICENSE
 * file that was distributed with the source code.
 */

#include "util.h"
#include "math/math.h"

/**
 * uniforms is done in two parts:
 *
 * 1) per group of shader programs (i.e.: per batch)
 * with UBO's (Uniform Buffer Object's), things like the projection matrix, which doesn't change between programs
 *
 * 2) per shader programs
 * with plain old uniforms (glUniform and friends)
 */
void gfxCreateRenderParams(struct gfxRenderParams *params) {
    memset(params, 0x0, sizeof(struct gfxRenderParams));

    /* no blending, culling, ... */
    params->blend = GFX_NONE;
    params->cull  = GFX_NONE;

    /* initialize the matrices to identity first */
    params->modelviewMatrix           = midentity();
    params->matrices.projectionMatrix = midentity();

    /* create UBO for global matrices */
    GLuint matrixUbo;

    glGenBuffers(1, &matrixUbo);
    glBindBuffer(GL_UNIFORM_BUFFER, matrixUbo);
    glBufferData(GL_UNIFORM_BUFFER, sizeof(struct gfxGlobalMatrices), &params->matrices, GL_DYNAMIC_DRAW); /* GL_STREAM_DRAW */
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    GL_ERROR("create ubo");

    params->matrixUbo = matrixUbo;
}

void gfxDestroyRenderParams(struct gfxRenderParams *params) {
    glDeleteBuffers(1, &params->matrixUbo);

    GL_ERROR("delete ubo");
}

void gfxBatch(const struct gfxRenderParams *params) {
    /* bind the static matrix stack to the correct binding point */
    glBindBufferRange(GL_UNIFORM_BUFFER, GFX_UBO_MATRICES, params->matrixUbo, 0, sizeof(struct gfxGlobalMatrices));
}
