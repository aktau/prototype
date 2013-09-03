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
};

struct gfxShaderProgram {
    unsigned int id;

    struct gfxUniformLocations loc;
    struct gfxShaderProgram *next;
};

struct gfxRenderParams {
    float projectionMatrix[16];

    unsigned char blend;
    unsigned char cull;
};

/*
struct gfxShaderParams {
    float modelviewMatrix[16];
};
*/

/* gfx/math.h */
void gfxVecToEuler(float* vec, float* euler);
void gfxVecNormalize(float* vec);
void gfxVecDotVec(float* vec1, float* vec2, float* dot);
void gfxVecCrossProduct(float* vec1, float* vec2, float* vec3);
float gfxVecLength(float* vec);
float gfxVecSquaredLength(float* vec);

void gfxQuaSetIdentity(float* qua);
void gfxQuaNormalize(float* qua, float* result);
void gfxQuaGetInverse(float* qua, float* invqua);
void gfxQuaFromAngleAxis(float angle, float* axis, float* qua);
void gfxQuaFromEuler(float x, float y, float z, float* qua);
void gfxQuaToMatrix4(float* qua, float* mat);
void gfxQuaToEuler(float* qua, float* euler);
void gfxRotateQua(float x, float y, float z, float* qua);
void gfxRotateQuaLocal(float x, float y, float z, float* qua);
void gfxMulQuaVec(float* qua, float* vec1, float* vec2);
void gfxMulQuaQua(float* qua1, float* qua2, float* qua3);
void gfxQuaSlerp(float* qa, float* qb, float t, float* result);

void gfxMatrix4SetIdentity(float* mat);
void gfxMatrix3SetIdentity(float* mat);
unsigned char gfxMatrix4GetInverse(float* mat1, float* mat2);
unsigned char gfxMatrix3GetInverse(float* mat1, float* mat2);
void gfxMatrix4GetTranspose(float* mat1, float* mat2);
void gfxMatrix3GetTranspose(float* mat1, float* mat2);
void gfxMatrix3ToQua(float* mat, float* qua);
void gfxMatrix4ToEuler(float* mat, float* eul);
void gfxMulMatrix4Vec3(float* m1, float* vec1, float* vec2);
void gfxMulMatrix4Vec4(float* m1, float* vec1, float* vec2);
void gfxMulMatrix4Matrix4(float* m1, float* m2, float* m3);
void gfxMulMatrix3Matrix4(float* m1, float* m2, float* m3);

unsigned char gfxBoxSphereIntersect(float* bmin, float* bmax, float* spos, float srad);

unsigned char gfxAabbInsideFrustum(float frustum[6][4], float* min, float* max);
unsigned char gfxSphereInsideFrustum(float frustum[6][4], float* pos, float radius);

#endif
