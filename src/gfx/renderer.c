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

static unsigned int gRenderParamsId;
static unsigned int gLayerId;

void gfxCreateLayer(struct gfxLayer *layer) {
    memset(layer, 0x0, sizeof(struct gfxLayer));

    GLuint ubo;
    glGenBuffers(1, &ubo);
    glBindBuffer(GL_UNIFORM_BUFFER, ubo);
    glBufferData(GL_UNIFORM_BUFFER, sizeof(struct gfxLayerUbo), &layer->uniforms, GL_DYNAMIC_DRAW); /* GL_STREAM_DRAW */
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
    GL_ERROR("create ubo");

    layer->uniforms.projectionMatrix = midentity();
    layer->uniforms.timer            = 0;
    layer->projection                = GFX_PERSPECTIVE;
    layer->ubo                       = ubo;
    layer->id                        = ++gLayerId;
}

/* call this when you've made changes to any of the uniforms of the layer */
void gfxUploadLayer(const struct gfxLayer *layer) {
    /* STATIC_ASSERT(sizeof(struct gfxLayerUbo) % sizeof(mat4) == 0, "uniform buffer storage is supposed to be aligned"); */

    glBindBuffer(GL_UNIFORM_BUFFER, layer->ubo);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(struct gfxLayerUbo), &layer->uniforms);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void gfxDestroyLayer(struct gfxLayer *layer) {
    glDeleteBuffers(1, &layer->ubo);
    GL_ERROR("delete ubo");
}

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

    /* initialize the matrices to identity first */
    params->modelviewMatrix = midentity();
    params->id = ++gRenderParamsId;

    /* no blending, culling, ... */
    params->blend = GFX_NONE;
    params->cull  = GFX_NONE;
}

void gfxDestroyRenderParams(struct gfxRenderParams *params) {}

void gfxBatch(const struct gfxLayer *layer) {
    /* bind the static matrix stack to the correct binding point */
    glBindBufferBase(GL_UNIFORM_BUFFER, GFX_UBO_LAYER, layer->ubo);

    /* alternatively:
     * glBindBufferRange(GL_UNIFORM_BUFFER, GFX_UBO_MATRICES, params->matrixUbo, 0, sizeof(struct gfxGlobalMatrices));
     *
     * NOTE: when using range, you have to query GL_UNIFORM_BUFFER_OFFSET_ALIGNMENT first:
     * http://stackoverflow.com/questions/13028852/issue-with-glbindbufferrange-opengl-3-1 */
}
