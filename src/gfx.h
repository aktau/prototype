/**
 * This file is part of prototype.
 *
 * (c) 2013 Nicolas Hillegeer <nicolas@hillegeer.com>
 *
 * For the full copyright and license information, please view the LICENSE
 * file that was distributed with the source code.
 */

#define GFX_VERTEX      0x0000
#define GFX_NORMAL      0x0001
#define GFX_TEXCOORD    0x0002
#define GFX_COLOR       0x0003
#define GFX_TANGENT     0x0004

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

struct gfxShaderProgram {
    struct gfxShaderProgram *next;

    unsigned int id;

    int projectionMatrixLoc;
    int invProjectionMatrixLoc;
    int modelviewMatrixLoc;
    int normalMatrixLoc;

    int texture0Loc;
    int texture1Loc;
    int texture2Loc;
    int texture3Loc;

    int colorMapLoc;
    int normalMapLoc;
    int heightMapLoc;
    int specularMapLoc;
    int colorRampMapLoc;
    int lightMapLoc;
    int cubeMapLoc;

    int shadowProjectionMatrixLoc;
    int shadowMapLoc;

    int ambientColorLoc;
    int diffuseColorLoc;
    int specularColorLoc;
    int shininessLoc;

    int lightPositionLoc;
    int lightColorLoc;

    int cameraPositionLoc;
};
