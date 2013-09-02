/**
 * This file is part of prototype.
 *
 * (c) 2013 Nicolas Hillegeer <nicolas@hillegeer.com>
 *
 * For the full copyright and license information, please view the LICENSE
 * file that was distributed with the source code.
 */

#include "util.h"

void gfxDestroyModel(struct gfxModel *model) {
    glBindVertexArray(model->vao);

    GL_ERROR("bind VAO");

    /* unbind buffers so we can safely delete them */
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    /* disable all vertex attrib arrays */
    for (GLuint i = 0; i < GFX_MAX_ATTRIB_ARRAY; ++i) {
        glDisableVertexAttribArray(i);

        GL_ERROR("disable VAA");
    }

    glDeleteBuffers(GFX_VBO_NUM, model->vbo);
    glDeleteBuffers(1, &model->ibo);

    GL_ERROR("delete buffers");

    glBindVertexArray(0);
    glDeleteVertexArrays(1, &model->vao);

    GL_ERROR("delete buffer objects");
}
