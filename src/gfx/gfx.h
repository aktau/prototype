/**
 * This file is part of prototype.
 *
 * (c) 2013 Nicolas Hillegeer <nicolas@hillegeer.com>
 *
 * For the full copyright and license information, please view the LICENSE
 * file that was distributed with the source code.
 */

#ifndef __gfx_h__
#define __gfx_h__

#include "math/types.h"

/* the maximum lenght of a glsl identifier. As far as I could find glsl
 * doesn't define a maximum. WebGL specifies 256 bytes, so we'll stick to
 * that. */
#define GFX_SHD_IDENT_SIZE_MAX  256

#define GFX_FALSE               0x0000
#define GFX_TRUE                0x0001

#define GFX_PI                  3.14159265f
#define GFX_PI_DIV_180          GFX_PI/180.0f
#define GFX_180_DIV_PI          180.0f/GFX_PI

#define GFX_VERTEX              0x0000
#define GFX_NORMAL              0x0001
#define GFX_TEXCOORD            0x0002
#define GFX_COLOR               0x0003
#define GFX_TANGENT             0x0004
#define GFX_MAX_ATTRIB_ARRAY    0x0005

/* blend modes */
#define GFX_NONE                0x0000
#define GFX_BLEND_ALPHA         0x0001
#define GFX_BLEND_PREMUL_ALPHA  0x0002

/* projection modes */
#define GFX_PERSPECTIVE         0x0000
#define GFX_ORTHO               0x0001

/* cull face modes */
/* #define GFX_NONE                0x0000 */
#define GFX_CULL_FRONT          0x0001
#define GFX_CULL_BACK           0x0002

#define GFX_UBO_LAYER           0x0001

typedef enum {
    GFX_VBO_VERTEX = 0,
    GFX_VBO_NORMAL,
    GFX_VBO_TEXCOORD,
    GFX_VBO_COLOR,
    GFX_VBO_TANGENT,
    GFX_VBO_NUM
} vbo_type_t;

struct gfxTransform {
    float position[3];
    float rotation[3];
    float scale[3];
    float orient[4];
    float matrix[16];
    float normalMatrix[9];
};

struct gfxTexture {
    unsigned int id;

    int width;
    int height;
    int format;
    int dataFormat;

    /* int type; */
};

struct gfxUniformLocations {
    int projectionMatrix;
    int invProjectionMatrix;
    int modelviewMatrix;
    int normalMatrix;

    int texture0;
    int texture1;
    int texture2;
    int texture3;

    int colorMap;
    int normalMap;
    int heightMap;
    int specularMap;
    int colorRampMap;
    int lightMap;
    int cubeMap;

    int shadowProjectionMatrix;
    int shadowMap;

    int ambientColor;
    int diffuseColor;
    int specularColor;
    int shininess;

    int lightPosition;
    int lightColor;

    int cameraPosition;

    int timer;

    unsigned int matricesBlockIndex;
};

struct gfxShaderProgram {
    unsigned int id;

    struct gfxUniformLocations loc;
    // struct gfxShaderProgram *next;
};

/* holds info about a uniform, like the size/stride/..., for now just used
 * for uniforms inside of a Uniform Buffer Objects, but that could change in
 * the future. */
struct gfxUniformInfo {
    char name[GFX_SHD_IDENT_SIZE_MAX];

    GLint type;
    GLint offset;
    GLint size;
    GLint arrayStride;
    GLint matrixStride;
};

/* describes the layout of a UBO buffer, will come in handy for debugging
 * and very necessary when we start uploading "shared" and "packed" UBOs. */
struct gfxUboLayout {
    size_t size;                       /* the block size, in bytes */
    char name[GFX_SHD_IDENT_SIZE_MAX]; /* the name of the block */

    size_t nelem;                    /* the number of uniforms in the buffer */
    struct gfxUniformInfo *uniforms; /* the info for every uniform in the buffer */
};

/* the C representation of a Uniform Buffer Object */
struct gfxGlobalMatrices {
    mat4 projectionMatrix;
};

/* each layer */
struct gfxLayerUbo {
    mat4 projectionMatrix;

    float timer;
};

/* per-layer attributes, such as projection matrix */
struct gfxLayer {
    struct gfxLayerUbo uniforms;
    unsigned int ubo;

    unsigned int id;

    /* what's the projection of this layer like?
     * GFX_PERSPECTIVE, GFX_ORTHO */
    unsigned char projection;
};

struct gfxRenderParams {
    mat4 modelviewMatrix;

    /* the id of this struct */
    unsigned int id;

    unsigned char blend;
    unsigned char cull;
};

/* best to allocate sub-arrays in one fell swoop or use a really good allocator */
struct gfxModel {
    unsigned int vao;
    unsigned int vbo[GFX_VBO_NUM];

    unsigned int ibo;
    int numIndices;

    unsigned int texture[1];
    unsigned int id;
};

struct gfxDrawOperation {
    /* regenerate this key whenever necessary (param change...) */
    union gfxDrawlistKey key;

    struct gfxModel *model;
    struct gfxRenderParams *params;
    struct gfxShaderProgram *program;
    struct gfxLayer *layer;
};

/* not in use yet, still deciding on the right format */
struct gfxRenderer {
    struct gfxLayer **layers;

    /* the maximum size in basic machine units (bytes) of a uniform block,
     * which must be at least 16384 */
    int uboMaxBlockSize;

    /* the maximum number of uniform buffer binding points on the context,
     * which must be at least 36 */
    int uboMaxBindings;

    /* ff you bind a uniform buffer with glBindBufferRange, the offset field
     * of that parameter must be a multiple of
     * GL_UNIFORM_BUFFER_OFFSET_ALIGNMENT. */
    int uboOffsetAlign;
};

/**
 * perf.c
 */

/* I read somewhere that some recent cards can have up to 5 frames of
 * latency (i.e.: the card runs 5 frames behind the CPU), let's put this to
 * the test. */
#define GFX_QUERY_FRAMES 3

/* enable this if you want the number of frames latency be made available
 * after calling gfxPerfFinishFrame(). */
/* #define GFX_PERF_NFRAMES_LATENCY */

/* how many queries do we use per frame? */
typedef enum {
    GFX_TIMER_RENDER = 0,
    GFX_TIMER_SWAP,
    GFX_PRIMITIVES_GENERATED,
    GFX_TIMER_NUM
} gfx_query_t;

/* a set of queries */
struct gfxQuerySet {
    GLuint queries[GFX_QUERY_FRAMES][GFX_TIMER_NUM];
    unsigned int curr;

#ifdef GFX_PERF_NFRAMES_LATENCY
    int numFramesLatency;
#endif
};

#endif

