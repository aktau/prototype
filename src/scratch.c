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

#define GFX_RED   { 1.0f, 0.0f, 0.0f, 1.0f }
#define GFX_GREEN { 0.0f, 1.0f, 0.0f, 1.0f }
#define GFX_BLUE  { 0.0f, 0.0f, 1.0f, 1.0f }

void gfxQuad(struct gfxModel *model) {
    memset(model, 0x0, sizeof(struct gfxModel));

    // const GLfloat vertices[] = {
    //     -0.5f, -0.5f, 0.0f, 1.0f,
    //     -0.5f, 0.5f, 0.0f, 1.0f,
    //     0.5f, -0.5f, 0.0f, 1.0f,
    //     0.5f, 0.5f, 0.0f, 1.0f
    // };

    const GLfloat vertices[] = {
        -0.7f, -0.7f, 0.0f, 1.0f,
        -0.7f, 0.7f, 0.0f, 1.0f,
        0.7f, -0.7f, 0.0f, 1.0f,
        0.7f, 0.7f, 0.0f, 1.0f
    };

    const GLfloat texcoords[] = {
        0, 0,
        0, 1,
        1, 0,
        1, 1
    };

    const GLubyte indices[] = {
        /* first triangle */
        0, 1, 2,
        /* second triangle */
        2, 1, 3
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

    /* send texcoords to GPU */
    glBindBuffer(GL_ARRAY_BUFFER, model->vbo[GFX_VBO_TEXCOORD]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(texcoords), texcoords, GL_STATIC_DRAW);
    glVertexAttribPointer(GFX_TEXCOORD, 2, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(GFX_TEXCOORD);

    GL_ERROR("load model VBO's");

    /* send vertex indices to the GPU */
    glGenBuffers(1, &model->ibo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, model->ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    model->numIndices = ARRAY_SIZE(indices);

    /* unbind to prevent modification */
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void gfxCube(struct gfxModel *model) {
    memset(model, 0x0, sizeof(struct gfxModel));

    static const struct {
        float vertices[4];
        float color[4];
    } staticData[] = {
        { { -.5f, -.5f,  .5f, 1 }, { 0, 0, 1, 1 } },
        { { -.5f,  .5f,  .5f, 1 }, { 1, 0, 0, 1 } },
        { {  .5f,  .5f,  .5f, 1 }, { 0, 1, 0, 1 } },
        { {  .5f, -.5f,  .5f, 1 }, { 1, 1, 0, 1 } },
        { { -.5f, -.5f, -.5f, 1 }, { 1, 1, 1, 1 } },
        { { -.5f,  .5f, -.5f, 1 }, { 1, 0, 0, 1 } },
        { {  .5f,  .5f, -.5f, 1 }, { 1, 0, 1, 1 } },
        { {  .5f, -.5f, -.5f, 1 }, { 0, 0, 1, 1 } }
    };

    const GLubyte indices[] = {
        0,2,1,  0,3,2,
        4,3,0,  4,7,3,
        4,1,5,  4,0,1,
        3,6,2,  3,7,6,
        1,6,5,  1,2,6,
        7,5,6,  7,4,5
    };

    /* generate and bind VAO */
    glGenVertexArrays(1, &(model->vao));
    glBindVertexArray(model->vao);

    GL_ERROR("create VAO");

    /* generate all VBO's */
    glGenBuffers(GFX_VBO_NUM, model->vbo);

    /* send vertices to GPU */
    glBindBuffer(GL_ARRAY_BUFFER, model->vbo[GFX_VBO_VERTEX]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(staticData), staticData, GL_STATIC_DRAW);
    // glVertexAttribPointer(GFX_VERTEX, 4, GL_FLOAT, GL_FALSE, 0, 0);
    glVertexAttribPointer(GFX_VERTEX, 4, GL_FLOAT, GL_FALSE, sizeof(staticData[0]), (GLvoid*) 0);
    glVertexAttribPointer(GFX_COLOR,  4, GL_FLOAT, GL_FALSE, sizeof(staticData[0]), (GLvoid*) sizeof(staticData[0].vertices));
    glEnableVertexAttribArray(GFX_VERTEX);
    glEnableVertexAttribArray(GFX_COLOR);

    /* colors */
    // glBindBuffer(GL_ARRAY_BUFFER, model->vbo[GFX_VBO_COLOR]);
    // glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    // glVertexAttribPointer(GFX_COLOR, 4, GL_FLOAT, GL_FALSE, 0, 0);
    // glEnableVertexAttribArray(GFX_COLOR);

    GL_ERROR("load model VBO's");

    /* send vertex indices to the GPU */
    glGenBuffers(1, &model->ibo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, model->ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    model->numIndices = ARRAY_SIZE(indices);

    /* unbind to prevent modification */
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

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

    model->numIndices = ARRAY_SIZE(indices);

    /* unbind to prevent modification */
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

#define AXIS_WIDTH          0.1f
#define AXIS_HALF_WIDTH     (AXIS_WIDTH * 0.5f)
#define AXIS_LENGTH         0.8f

void gfxAxis(struct gfxModel *model) {
    memset(model, 0x0, sizeof(struct gfxModel));

    static const struct {
        float vertices[4];
        float color[4];
    } staticData[] = {
        /* x-axis */
        { { 0.0f, 0.0f, 0.0f, 1.0f }, GFX_RED },
        { { AXIS_LENGTH, 0.0f, AXIS_HALF_WIDTH, 1.0f }, GFX_RED },
        { { AXIS_LENGTH, 0.0f, -AXIS_HALF_WIDTH, 1.0f }, GFX_RED },
        { { AXIS_LENGTH, AXIS_HALF_WIDTH, 0.0f, 1.0f }, GFX_RED },
        { { AXIS_LENGTH, -AXIS_HALF_WIDTH, 0.0f, 1.0f }, GFX_RED },

        /* y-axis */
        { { 0.0f, 0.0f, 0.0f, 1.0f }, GFX_GREEN },
        { { 0.0f, AXIS_LENGTH, AXIS_HALF_WIDTH, 1.0f }, GFX_GREEN },
        { { 0.0f, AXIS_LENGTH, -AXIS_HALF_WIDTH, 1.0f }, GFX_GREEN },
        { { AXIS_HALF_WIDTH, AXIS_LENGTH, 0.0f, 1.0f }, GFX_GREEN },
        { { -AXIS_HALF_WIDTH, AXIS_LENGTH, 0.0f, 1.0f }, GFX_GREEN },

        /* z-axis */
        { { 0.0f, 0.0f, 0.0f, 1.0f }, GFX_BLUE },
        { { 0.0f, AXIS_HALF_WIDTH, AXIS_LENGTH, 1.0f }, GFX_BLUE },
        { { 0.0f, AXIS_HALF_WIDTH, AXIS_LENGTH, 1.0f }, GFX_BLUE },
        { { AXIS_HALF_WIDTH, 0.0f, AXIS_LENGTH, 1.0f }, GFX_BLUE },
        { { -AXIS_HALF_WIDTH, 0.0f, AXIS_LENGTH, 1.0f }, GFX_BLUE }
    };

    const GLubyte indices[] = {
        0, 3, 2,    0, 1, 3,
        0, 4, 1,    0, 2, 4,
        2, 3, 1,    1, 4, 2,

        5, 8, 7,    5, 6, 8,
        5, 9, 6,    5, 7, 9,
        7, 8, 6,    6, 9, 7,

        10, 13, 12,    10, 11, 13,
        10, 14, 11,    10, 12, 14,
        11, 13, 12,    14, 11, 12
    };


    /* generate and bind VAO */
    glGenVertexArrays(1, &(model->vao));
    glBindVertexArray(model->vao);

    GL_ERROR("create VAO");

    /* generate all VBO's */
    glGenBuffers(GFX_VBO_NUM, model->vbo);

    /* send vertices to GPU */
    glBindBuffer(GL_ARRAY_BUFFER, model->vbo[GFX_VBO_VERTEX]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(staticData), staticData, GL_STATIC_DRAW);
    glVertexAttribPointer(GFX_VERTEX, 4, GL_FLOAT, GL_FALSE, sizeof(staticData[0]), (GLvoid*) 0);
    glVertexAttribPointer(GFX_COLOR,  4, GL_FLOAT, GL_FALSE, sizeof(staticData[0]), (GLvoid*) sizeof(staticData[0].vertices));
    glEnableVertexAttribArray(GFX_VERTEX);
    glEnableVertexAttribArray(GFX_COLOR);

    GL_ERROR("load model VBO's");

    /* send vertex indices to the GPU */
    glGenBuffers(1, &model->ibo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, model->ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    model->numIndices = ARRAY_SIZE(indices);

    /* unbind to prevent modification */
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

/**
 * generates a pre-tesselated sheet, do with it as you please in the shader
 *
 * TODO: technically we could make the input to the shader a vec2, which
 * would cut our data by almost 50%, which is nice especially for huge
 * sheets
 *
 * TODO: it might be faster to just make it a pure triangle strip with
 * some degenerate triangles, this would allow the driver to cut out the
 * index lookup and save some bytes at the same time. This is done here:
 * http://www.learnopengles.com/android-lesson-eight-an-introduction-to-index-buffer-objects-ibos/
 * http://www.chadvernon.com/blog/resources/directx9/terrain-generation-with-a-heightmap/
 * http://www.catalinzima.com/tutorials/4-uses-of-vtf/terrain-rendering-using-heightmaps/
 *
 * primitive restarts can enhance the performance of both triangle strip and triangle fan
 * heightmap rendering:
 * http://www.opengl.org/registry/specs/NV/primitive_restart.txt
 */
void gfxSheet(struct gfxModel *model, float width, float height, unsigned int subdiv) {
    memset(model, 0x0, sizeof(struct gfxModel));

    /* vertex * number of vertices */
    const size_t nverts = (subdiv + 1) * (subdiv + 1);
    const size_t vsize  = (sizeof(GLfloat) * 4) * nverts;
    GLfloat *vertices   = zmalloc(vsize);
    GLfloat *vertex     = vertices;

    const GLubyte stride = (GLubyte)(subdiv + 1);

    const float startx = 0.0f;
    const float starty = 0.0f;
    const float startz = -0.5f;

    const float tileWidth  = width / (float) subdiv;
    const float tileHeight = height / (float) subdiv;

    int counter = 0;

    for (int i = 0; i < (int) subdiv + 1; ++i) {
        for (int j = 0; j < (int) subdiv + 1; ++j) {
            vertex[0] = startx + i * tileWidth;
            vertex[1] = starty; // + (i * tileWidth - j * tileHeight);
            vertex[2] = startz - j * tileHeight; //startz;
            vertex[3] = 1.0f;

            trace("generated vertex %d: [%3.2f %3.2f %3.2f %3.2f]\n", counter, vertex[0], vertex[1], vertex[2], vertex[3]);
            ++counter;

            vertex += 4;

        }
    }

    /* triangle * number of triangles  */
    const size_t isize = (sizeof(GLubyte) * 3) * (subdiv * subdiv * 2);
    GLubyte *indices   = zmalloc(isize);
    GLubyte *index     = indices;

    counter = 0;

    for (int i = 0; i < (int) subdiv; ++i) {
        for (int j = 0; j < (int) subdiv; ++j) {
            /**
             * tile (i,j) has 2 triangles, which means 6 indexes
             *
             * the indices of the vertices for a tile (i,j) are:
             *
             * offset = i * stride + j
             *
             * lower-left  = offset + 0
             * lower-right = offset + stride
             * upper-left  = offset + 1
             * upper-right = offset + stride + 1
             */
            const GLubyte offset = (GLubyte)(i * stride + j);

            /* first triangle */
            index[0] = offset;
            index[1] = offset + 1;
            index[2] = offset + stride;

            index += 3;

            /* second triangle */
            index[0] = offset + 1;
            index[1] = offset + stride + 1;
            index[2] = offset + stride;

            index += 3;

            trace("quad %d: [%u %u %u] [%u %u %u]\n", counter, index[-6], index[-5], index[-4], index[-3], index[-2], index[-1]);
            ++counter;
        }

        /* insert degenerate triangle, only when generating triangle strips */
    }

    /* generate and bind VAO */
    glGenVertexArrays(1, &(model->vao));
    glBindVertexArray(model->vao);

    GL_ERROR("create VAO");

    /* generate all VBO's */
    glGenBuffers(GFX_VBO_NUM, model->vbo);

    /* send vertices to GPU */
    glBindBuffer(GL_ARRAY_BUFFER, model->vbo[GFX_VBO_VERTEX]);
    glBufferData(GL_ARRAY_BUFFER, (GLsizeiptr) vsize, vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(GFX_VERTEX, 4, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(GFX_VERTEX);

    GL_ERROR("load model VBO's");

    /* send vertex indices to the GPU */
    glGenBuffers(1, &model->ibo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, model->ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, (GLsizeiptr) isize, indices, GL_STATIC_DRAW);

    trace("loaded %lu indices (%lu bytes) and %lu vertices (%lu bytes). (%lu bytes total)\n", isize, isize, nverts, vsize, isize + vsize);
    model->numIndices = (int) isize;

    /* unbind to prevent modification */
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    zfree(vertices);
    zfree(indices);
}
