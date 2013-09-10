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

/* cull face modes */
/* #define GFX_NONE                0x0000 */
#define GFX_CULL_FRONT          0x0001
#define GFX_CULL_BACK           0x0002

#define GFX_UBO_MATRICES        0x0001

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

/* the C representation of a Uniform Buffer Object */
struct gfxGlobalMatrices {
    float projectionMatrix[16];
};

struct gfxRenderParams {
    float modelviewMatrix[16];

    /* the uniform buffer object that stores matrices common to all (or most) programs */
    struct gfxGlobalMatrices matrices; /* the C side */
    unsigned int matrixUbo; /* the OpenGL side */

    unsigned char blend;
    unsigned char cull;

    /* relative timer inmilliseconds */
    float timer;
};

#endif
