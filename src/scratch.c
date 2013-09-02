/**
 * This file is part of prototype.
 *
 * (c) 2013 Nicolas Hillegeer <nicolas@hillegeer.com>
 *
 * For the full copyright and license information, please view the LICENSE
 * file that was distributed with the source code.
 *
 * This file contains miscellaneous functions that are nice to (stress)-test
 * the engine and load simple models without loading a file, but will not be
 * used be an actual game
 */

#include "util.h"

void gfxCrystal(struct gfxModel *model) {
    memset(model, 0x0, sizeof(struct gfxModel));

    const GLfloat vertices[] = {
        0.0f, 0.0f, 0.0f, 1.0f,
        /* top */
        -0.2f, 0.8f, 0.0f, 1.0f,
        0.2f, 0.8f, 0.0f, 1.0f,
        0.0f, 0.8f, 0.0f, 1.0f,
        0.0f, 1.0f, 0.0f, 1.0f,
        /* bottom */
        -0.2f, -0.8f, 0.0f, 1.0f,
        0.2f, -0.8f, 0.0f, 1.0f,
        0.0f, -0.8f, 0.0f, 1.0f,
        0.0f, -1.0f, 0.0f, 1.0f,
        /* left */
        -0.8f, -0.2f, 0.0f, 1.0f,
        -0.8f, 0.2f, 0.0f, 1.0f,
        -0.8f, 0.0f, 0.0f, 1.0f,
        -1.0f, 0.0f, 0.0f, 1.0f,
        /* right */
        0.8f, -0.2f, 0.0f, 1.0f,
        0.8f, 0.2f, 0.0f, 1.0f,
        0.8f, 0.0f, 0.0f, 1.0f,
        1.0f, 0.0f, 0.0f, 1.0f
    };

    const GLfloat colors[] = {
        1.0f, 1.0f, 1.0f, 1.0f,
        0.0f, 1.0f, 0.0f, 1.0f,
        0.0f, 0.0f, 1.0f, 1.0f,
        0.0f, 1.0f, 1.0f, 1.0f,
        1.0f, 0.0f, 0.0f, 1.0f,
        0.0f, 0.0f, 1.0f, 1.0f,
        0.0f, 1.0f, 0.0f, 1.0f,
        0.0f, 1.0f, 1.0f, 1.0f,
        1.0f, 0.0f, 0.0f, 1.0f,
        0.0f, 1.0f, 0.0f, 1.0f,
        0.0f, 0.0f, 1.0f, 1.0f,
        0.0f, 1.0f, 1.0f, 1.0f,
        1.0f, 0.0f, 0.0f, 1.0f,
        0.0f, 0.0f, 1.0f, 1.0f,
        0.0f, 1.0f, 0.0f, 1.0f,
        0.0f, 1.0f, 1.0f, 1.0f,
        1.0f, 0.0f, 0.0f, 1.0f
    };

    const GLubyte indices[] = {
        /* top */
        0, 1, 3,
        0, 3, 2,
        3, 1, 4,
        3, 4, 2,
        /* bottom */
        0, 5, 7,
        0, 7, 6,
        7, 5, 8,
        7, 8, 6,
        /* left */
        0, 9, 11,
        0, 11, 10,
        11, 9, 12,
        11, 12, 10,
        /* right */
        0, 13, 15,
        0, 15, 14,
        15, 13, 16,
        15, 16, 14
    };

    /* generate and bind VAO */
    glGenVertexArrays(1, &(model->vao));
    glBindVertexArray(model->vao);

    GL_ERROR("create VAO");

    /* generate all VBO's */
    glGenBuffers(GFX_VBO_NUM, model->vbo);

    /* send vertices to GPU */
    glBindBuffer(GL_ARRAY_BUFFER, model->vbo[GFX_VBO_VERTEX]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(GFX_VERTEX, 4, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(GFX_VERTEX);

    /* send normals to GPU */
    glBindBuffer(GL_ARRAY_BUFFER, model->vbo[GFX_VBO_NORMAL]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(GFX_NORMAL, 4, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(GFX_NORMAL);

    /* send texcoords to GPU */
    glBindBuffer(GL_ARRAY_BUFFER, model->vbo[GFX_VBO_TEXCOORD]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(GFX_TEXCOORD, 4, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(GFX_TEXCOORD);

    /* send colors to GPU */
    glBindBuffer(GL_ARRAY_BUFFER, model->vbo[GFX_VBO_COLOR]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(GFX_COLOR, 4, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(GFX_COLOR);

    GL_ERROR("load model VBO's");

    /* send vertex indices to the GPU */
    glGenBuffers(1, &model->ibo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, model->ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    /* unbind to prevent modification */
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}
