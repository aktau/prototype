/**
 * This file is part of prototype.
 *
 * Copyright (C) 2009-2010 Samuel Anjam
 * Copyright (C) 2013 Nicolas Hillegeer <nicolas@hillegeer.com>
 *
 * This software is provided 'as-is', without any express or implied
 * warranty.  In no event will the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software
 *    in a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 */

#ifndef __gfx_math_h__
#define __gfx_math_h__

#include "util.h"

void gfxVecToEuler(float* vec, float* euler)
{
    //euler[0] = -GFX_180_DIV_PI*atan(sqrt(vec[0]*vec[0]+vec[1]*vec[1])/vec[2]);
    euler[0] = 0.0f;
    euler[1] = 0.0f;
    euler[2] = -GFX_180_DIV_PI*atanf(vec[0]/(vec[1]));
}

void gfxVecNormalize(float* vec)
{
    float length = 1.0f/sqrtf(vec[0]*vec[0]+vec[1]*vec[1]+vec[2]*vec[2]);
    vec[0] *= length;
    vec[1] *= length;
    vec[2] *= length;
}

void gfxVecDotVec(float* vec1, float* vec2, float* dot)
{
    *dot = vec1[0]*vec2[0]+vec1[1]*vec2[1]+vec1[2]*vec2[2];
}

void gfxVecCrossProduct(float* vec1, float* vec2, float* vec3)
{
    vec3[0] = vec1[1]*vec2[2]-vec1[2]*vec2[1];
    vec3[1] = vec1[2]*vec2[0]-vec1[0]*vec2[2];
    vec3[2] = vec1[0]*vec2[1]-vec1[1]*vec2[0];
}

float gfxVecLength(float* vec)
{
    return (float)sqrt(vec[0]*vec[0]+vec[1]*vec[1]+vec[2]*vec[2]);
}

float gfxVecSquaredLength(float* vec)
{
    return vec[0]*vec[0]+vec[1]*vec[1]+vec[2]*vec[2];
}

void gfxQuaSetIdentity(float* qua)
{
    qua[0] = qua[1] = qua[2] = 0.0f;
    qua[3] = 1.0f;
}

void gfxQuaNormalize(float* qua, float* result)
{
    float length = 1.0f/sqrtf(qua[0]*qua[0]+qua[1]*qua[1]+qua[2]*qua[2]+qua[3]*qua[3]);
    result[0] = qua[0]*length;
    result[1] = qua[1]*length;
    result[2] = qua[2]*length;
    result[3] = qua[3]*length;
}

void gfxQuaGetInverse(float* qua, float* invqua)
{
    float length = 1.0f/(qua[0]*qua[0]+qua[1]*qua[1]+qua[2]*qua[2]+qua[3]*qua[3]);
    invqua[0] = qua[0]*(-length);
    invqua[1] = qua[1]*(-length);
    invqua[2] = qua[2]*(-length);
    invqua[3] = qua[3]*length;
}

void gfxQuaFromAngleAxis(float angle, float* axis, float* qua)
{
    float sinA;
    float cosA;

    sinA = sinf(angle/2.0f*GFX_PI_DIV_180);
    cosA = cosf(angle/2.0f*GFX_PI_DIV_180);

    qua[0] = axis[0]*sinA;
    qua[1] = axis[1]*sinA;
    qua[2] = axis[2]*sinA;
    qua[3] = cosA;
}

void gfxQuaFromEuler(float x, float y, float z, float* qua)
{
    float axisAngleQua[4];
    float tempQua[4];
    float axis[3];

    gfxQuaSetIdentity(qua);
    memset(axis, 0x0, sizeof(float)*3);

    if(x != 0.0f)
    {
        axis[0] = 1.0f; axis[1] = axis[2] = 0.0f;
        gfxQuaFromAngleAxis(x, axis, axisAngleQua);
        gfxMulQuaQua(axisAngleQua, qua, tempQua);
        memcpy(qua, tempQua, sizeof(float)*4);
    }
    if(y != 0.0f)
    {
        axis[1] = 1.0f; axis[0] = axis[2] = 0.0f;
        gfxQuaFromAngleAxis(y, axis, axisAngleQua);
        gfxMulQuaQua(axisAngleQua, qua, tempQua);
        memcpy(qua, tempQua, sizeof(float)*4);
    }
    if(z != 0.0f)
    {
        axis[2] = 1.0f; axis[0] = axis[1] = 0.0f;
        gfxQuaFromAngleAxis(z, axis, axisAngleQua);
        gfxMulQuaQua(axisAngleQua, qua, tempQua);
        memcpy(qua, tempQua, sizeof(float)*4);
    }
}

void gfxQuaToMatrix4(float* qua, float* mat)
{
    float xx = 2*qua[0]*qua[0];
    float xy = 2*qua[0]*qua[1];
    float xz = 2*qua[0]*qua[2];
    float xw = 2*qua[0]*qua[3];
    float yy = 2*qua[1]*qua[1];
    float yz = 2*qua[1]*qua[2];
    float yw = 2*qua[1]*qua[3];
    float zz = 2*qua[2]*qua[2];
    float zw = 2*qua[2]*qua[3];
    mat[0] = 1-yy-zz; mat[1] = xy-zw; mat[2] = xz+yw;
    mat[4] = xy+zw; mat[5] = 1-xx-zz; mat[6] = yz-xw;
    mat[8] = xz-yw; mat[9] = yz+xw; mat[10] = 1-xx-yy;
    mat[12] = mat[13] = mat[14] = mat[3] = mat[7] = mat[11] = 0.0f;
    mat[15] = 1.0f;
}

void gfxQuaToEuler(float* qua, float* euler)
{
    float sqx = qua[0]*qua[0];
    float sqy = qua[1]*qua[1];
    float sqz = qua[2]*qua[2];
    float sqw = qua[3]*qua[3];

    euler[0] = atan2f(2.0f*(qua[1]*qua[2]+qua[0]*qua[3]), -sqx-sqy+sqz+sqw);
    euler[1] = asinf(-2.0f*(qua[0]*qua[2]-qua[1]*qua[3]));
    euler[2] = atan2f(2.0f*(qua[0]*qua[1]+qua[2]*qua[3]), sqx-sqy-sqz+sqw);

    euler[0] *= GFX_180_DIV_PI;
    euler[1] *= GFX_180_DIV_PI;
    euler[2] *= GFX_180_DIV_PI;
}

void gfxRotateQua(float x, float y, float z, float* qua)
{
    float axisAngleQua[4];
    float tempQua0[4];
    float tempQua1[4];
    float axis[3];

    gfxQuaSetIdentity(tempQua0);
    memset(axis, 0x0, sizeof(float)*3);

    if(x != 0.0f)
    {
        axis[0] = 1.0f; axis[1] = axis[2] = 0.0f;
        gfxQuaFromAngleAxis(x, axis, axisAngleQua);
        gfxMulQuaQua(axisAngleQua, tempQua0, tempQua1);
        memcpy(tempQua0, tempQua1, sizeof(float)*4);
    }
    if(y != 0.0f)
    {
        axis[1] = 1.0f; axis[0] = axis[2] = 0.0f;
        gfxQuaFromAngleAxis(y, axis, axisAngleQua);
        gfxMulQuaQua(axisAngleQua, tempQua0, tempQua1);
        memcpy(tempQua0, tempQua1, sizeof(float)*4);
    }
    if(z != 0.0f)
    {
        axis[2] = 1.0f; axis[0] = axis[1] = 0.0f;
        gfxQuaFromAngleAxis(z, axis, axisAngleQua);
        gfxMulQuaQua(axisAngleQua, tempQua0, tempQua1);
        memcpy(tempQua0, tempQua1, sizeof(float)*4);
    }

    memcpy(tempQua1, qua, sizeof(float)*4);
    gfxMulQuaQua(tempQua0, tempQua1, qua);
}

void gfxRotateQuaLocal(float x, float y, float z, float* qua)
{
    float axisAngleQua[4];
    float tempQua0[4];
    float tempQua1[4];
    float tempAxis[3];
    float axis[3];

    gfxQuaSetIdentity(tempQua0);
    memset(axis, 0x0, sizeof(float)*3);

    if(x != 0.0f)
    {
        tempAxis[0] = 1.0f; tempAxis[1] = tempAxis[2] = 0.0f;
        gfxMulQuaVec(qua, tempAxis, axis);
        gfxQuaFromAngleAxis(x, axis, axisAngleQua);
        gfxMulQuaQua(axisAngleQua, tempQua0, tempQua1);
        memcpy(tempQua0, tempQua1, sizeof(float)*4);
    }
    if(y != 0.0f)
    {
        tempAxis[1] = 1.0f; tempAxis[0] = tempAxis[2] = 0.0f;
        gfxMulQuaVec(qua, tempAxis, axis);
        gfxQuaFromAngleAxis(y, axis, axisAngleQua);
        gfxMulQuaQua(axisAngleQua, tempQua0, tempQua1);
        memcpy(tempQua0, tempQua1, sizeof(float)*4);
    }
    if(z != 0.0f)
    {
        tempAxis[2] = 1.0f; tempAxis[0] = tempAxis[1] = 0.0f;
        gfxMulQuaVec(qua, tempAxis, axis);
        gfxQuaFromAngleAxis(z, axis, axisAngleQua);
        gfxMulQuaQua(axisAngleQua, tempQua0, tempQua1);
        memcpy(tempQua0, tempQua1, sizeof(float)*4);
    }

    memcpy(tempQua1, qua, sizeof(float)*4);
    gfxMulQuaQua(tempQua0, tempQua1, qua);
}

void gfxMulQuaVec(float* qua, float* vec1, float* vec2)
{
    float uv[3];
    float uuv[3];
    float qvec[3];
    float qmul;

    qvec[0] = qua[0]; qvec[1] = qua[1]; qvec[2] = qua[2];

    gfxVecCrossProduct(qvec, vec1, uv);
    gfxVecCrossProduct(qvec, uv, uuv);

    qmul = 2.0f*qua[3];
    uv[0] *= qmul;
    uv[1] *= qmul;
    uv[2] *= qmul;

    uuv[0] *= 2.0f;
    uuv[1] *= 2.0f;
    uuv[2] *= 2.0f;

    vec2[0] = vec1[0]+uv[0]+uuv[0];
    vec2[1] = vec1[1]+uv[1]+uuv[1];
    vec2[2] = vec1[2]+uv[2]+uuv[2];
}

void gfxMulQuaQua(float* qua1, float* qua2, float* qua3)
{
    qua3[0] =  qua1[0]*qua2[3]+qua1[1]*qua2[2]-qua1[2]*qua2[1]+qua1[3]*qua2[0];
    qua3[1] = -qua1[0]*qua2[2]+qua1[1]*qua2[3]+qua1[2]*qua2[0]+qua1[3]*qua2[1];
    qua3[2] =  qua1[0]*qua2[1]-qua1[1]*qua2[0]+qua1[2]*qua2[3]+qua1[3]*qua2[2];
    qua3[3] = -qua1[0]*qua2[0]-qua1[1]*qua2[1]-qua1[2]*qua2[2]+qua1[3]*qua2[3];
}

void gfxQuaSlerp(float* qa, float* qc, float t, float* result)
{
    float cosHalfTheta;
    float halfTheta;
    float sinHalfTheta;
    float ratioA;
    float ratioB;
    float qb[4];

    memcpy(qb, qc, sizeof(float)*4);

    cosHalfTheta = qa[3]*qb[3]+qa[0]*qb[0]+qa[1]*qb[1]+qa[2]*qb[2];
    if(fabsf(cosHalfTheta) >= 1.0f)
    {
        result[0] = qa[0];
        result[1] = qa[1];
        result[2] = qa[2];
        result[3] = qa[3];
        return;
    }

    if (cosHalfTheta < 0.0f)
    {
        qb[0] = -qb[0];
        qb[1] = -qb[1];
        qb[2] = -qb[2];
        qb[3] = -qb[3];
        cosHalfTheta = -cosHalfTheta;
    }

    halfTheta = acosf(cosHalfTheta);
    sinHalfTheta = sqrtf(1.0f-cosHalfTheta*cosHalfTheta);

    if(fabsf(sinHalfTheta) < 0.001f)
    {
        result[0] = (qa[0] * 0.5f + qb[0] * 0.5f);
        result[1] = (qa[1] * 0.5f + qb[1] * 0.5f);
        result[2] = (qa[2] * 0.5f + qb[2] * 0.5f);
        result[3] = (qa[3] * 0.5f + qb[3] * 0.5f);
        return;
    }

    ratioA = sinf((1 - t) * halfTheta) / sinHalfTheta;
    ratioB = sinf(t * halfTheta) / sinHalfTheta;

    result[0] = (qa[0] * ratioA + qb[0] * ratioB);
    result[1] = (qa[1] * ratioA + qb[1] * ratioB);
    result[2] = (qa[2] * ratioA + qb[2] * ratioB);
    result[3] = (qa[3] * ratioA + qb[3] * ratioB);
}

void gfxMatrix4SetIdentity(float* mat)
{
    memset(mat, 0x0, sizeof(float)*16);
    mat[0] = mat[5] = mat[10] = mat[15] = 1.0f;
}

void gfxMatrix3SetIdentity(float* mat)
{
    memset(mat, 0x0, sizeof(float)*9);
    mat[0] = mat[4] = mat[8] = 1.0f;
}

#define MATSWAP(a,b) {temp=(a);(a)=(b);(b)=temp;}

unsigned char gfxMatrix4GetInverse(float* mat1, float* mat2)
{
    float matr[4][4], ident[4][4];
    int i, j, k, l, ll;
    int icol=0, irow=0;
    int indxc[4], indxr[4], ipiv[4];
    float big, dum, pivinv, temp;

    for(i = 0; i < 4; i++)
    {
        for (j=0; j<4; j++)
        {
            matr[i][j] = mat1[4*i+j];
            ident[i][j] = 0.0f;
        }
        ident[i][i] = 1.0f;
    }

    for(j = 0; j <= 3; j++) ipiv[j] = 0;
    for(i = 0; i <= 3; i++)
    {
        big = 0.0f;

        for (j = 0; j <= 3; j++)
        {
            if(ipiv[j] != 1)
            {
                for(k = 0; k <= 3; k++)
                {
                    if(ipiv[k] == 0)
                    {
                        if(fabs(matr[j][k]) >= big)
                        {
                            big = fabsf(matr[j][k]);
                            irow = j;
                            icol = k;
                        }
                    }
                    else if(ipiv[k] > 1)
                    {
                        return GFX_FALSE;
                    }
                }
            }
        }

        ++(ipiv[icol]);

        if(irow != icol)
        {
            for(l = 0; l <= 3;l++) MATSWAP(matr[irow][l], matr[icol][l]);
            for(l = 0; l <= 3;l++) MATSWAP(ident[irow][l], ident[icol][l]);
        }

        indxr[i]=irow;
        indxc[i]=icol;

        if(matr[icol][icol] == 0.0f) return GFX_FALSE;

        pivinv = 1.0f / matr[icol][icol];
        matr[icol][icol] = 1.0f;

        for(l = 0; l <= 3 ; l++) matr[icol][l] *= pivinv;
        for(l = 0; l <= 3 ; l++) ident[icol][l] *= pivinv;

        for(ll = 0; ll <= 3; ll++)
        {
            if (ll != icol)
            {
                dum=matr[ll][icol];
                matr[ll][icol] = 0.0f;
                for(l = 0; l<=3; l++) matr[ll][l] -= matr[icol][l]*dum;
                for(l = 0; l<=3; l++) ident[ll][l] -= ident[icol][l]*dum;
            }
        }
    }

    for(l = 3; l >= 0; l--)
    {
        if(indxr[l] != indxc[l])
        {
            for(k = 0; k <= 3; k++)
            {
                MATSWAP(matr[k][indxr[l]], matr[k][indxc[l]]);
            }
        }
    }

    for(i = 0; i < 4; i++)
    {
        for(j = 0; j < 4; j++)
        {
            mat2[4*i+j] = matr[i][j];
        }
    }

    return GFX_TRUE;
}

unsigned char gfxMatrix3GetInverse(float* mat1, float* mat2)
{
    float matr[3][3], ident[3][3];
    int i, j, k, l, ll;
    int icol=0, irow=0;
    int indxc[3], indxr[3], ipiv[3];
    float big, dum, pivinv, temp;

    for(i = 0; i < 3; i++)
    {
        for (j = 0; j < 3; j++)
        {
            matr[i][j] = mat1[3*i+j];
            ident[i][j] = 0.0f;
        }
        ident[i][i] = 1.0f;
    }

    for(j = 0; j <= 2; j++) ipiv[j] = 0;
    for(i = 0; i <= 2; i++)
    {
        big = 0.0f;

        for (j = 0; j <= 2; j++)
        {
            if(ipiv[j] != 1)
            {
                for(k = 0; k <= 2; k++)
                {
                    if(ipiv[k] == 0)
                    {
                        if(fabsf(matr[j][k]) >= big)
                        {
                            big = fabsf(matr[j][k]);
                            irow = j;
                            icol = k;
                        }
                    }
                    else if(ipiv[k] > 1)
                    {
                        return GFX_FALSE;
                    }
                }
            }
        }

        ++(ipiv[icol]);

        if(irow != icol)
        {
            for(l = 0; l <= 2; l++) MATSWAP(matr[irow][l], matr[icol][l]);
            for(l = 0; l <= 2; l++) MATSWAP(ident[irow][l], ident[icol][l]);
        }

        indxr[i]=irow;
        indxc[i]=icol;

        if(matr[icol][icol] == 0.0f) return GFX_FALSE;

        pivinv = 1.0f / matr[icol][icol];
        matr[icol][icol] = 1.0f;

        for(l = 0; l <= 2; l++) matr[icol][l] *= pivinv;
        for(l = 0; l <= 2; l++) ident[icol][l] *= pivinv;

        for(ll = 0; ll <= 2; ll++)
        {
            if (ll != icol)
            {
                dum=matr[ll][icol];
                matr[ll][icol] = 0.0f;
                for(l = 0; l<=2; l++) matr[ll][l] -= matr[icol][l]*dum;
                for(l = 0; l<=2; l++) ident[ll][l] -= ident[icol][l]*dum;
            }
        }
    }

    for(l = 2; l >= 0; l--)
    {
        if(indxr[l] != indxc[l])
        {
            for(k = 0; k <= 2; k++)
            {
                MATSWAP(matr[k][indxr[l]], matr[k][indxc[l]]);
            }
        }
    }

    for(i = 0; i < 3; i++)
    {
        for(j = 0; j < 3; j++)
        {
            mat2[3*i+j] = matr[i][j];
        }
    }

    return GFX_TRUE;
}

/**
 * I'm sure there are better ways to do this, the original
 * function doesn't appear to be very well tested because
 * it used "int" for the save variable. If necessary could
 * also consider to make an in-place version
 */
void gfxMatrix4GetTranspose(float* mat1, float* mat2)
{
    int i, j;
    float save;
    float matrix[4][4];

    for(i = 0; i < 4; i++)
    {
        for(j = 0; j < 4; j++)
        {
            matrix[i][j] = mat1[4*i+j];
        }
    }

    for(i = 0; i < 4; i++)
    {
        for(j = i + 1; j < 4; j++)
        {
            save = matrix[i][j];
            matrix[i][j] = matrix[j][i];
            matrix[j][i] = save;
        }
    }

    for(i = 0; i < 4; i++)
    {
        for(j = 0; j < 4; j++)
        {
            mat2[4*i+j] = matrix[i][j];
        }
    }
}

void gfxMatrix3GetTranspose(float* mat1, float* mat2)
{
    int i, j;
    float save;
    float matrix[3][3];

    for(i = 0; i < 3; i++)
    {
        for(j = 0; j < 3; j++)
        {
            matrix[i][j] = mat1[3*i+j];
        }
    }

    for(i = 0; i < 3; i++)
    {
        for(j = i + 1; j < 3; j++)
        {
            save = matrix[i][j];
            matrix[i][j] = matrix[j][i];
            matrix[j][i] = save;
        }
    }

    for(i = 0; i < 3; i++)
    {
        for(j = 0; j < 3; j++)
        {
            mat2[3*i+j] = matrix[i][j];
        }
    }
}

void gfxMatrix3ToQua(float* mat, float* qua)
{
    float S;
    float tr = mat[0] + mat[4] + mat[8];

    if (tr > 0)
    {
        S = sqrtf(tr+1.0f) * 2.0f;
        qua[3] = 0.25f * S;
        qua[0] = (mat[7] - mat[5]) / S;
        qua[1] = (mat[2] - mat[6]) / S;
        qua[2] = (mat[3] - mat[1]) / S;
    }
    else if(mat[0] > mat[4] && mat[0] > mat[8])
    {
        S = sqrtf(1.0f + mat[0] - mat[4] - mat[8]) * 2.0f;
        qua[3] = (mat[7] - mat[5]) / S;
        qua[0] = 0.25f * S;
        qua[1] = (mat[1] + mat[3]) / S;
        qua[2] = (mat[2] + mat[6]) / S;
    }
    else if(mat[4] > mat[8])
    {
        S = sqrtf(1.0f + mat[4] - mat[0] - mat[8]) * 2.0f;
        qua[3] = (mat[2] - mat[6]) / S;
        qua[0] = (mat[1] + mat[3]) / S;
        qua[1] = 0.25f * S;
        qua[2] = (mat[5] + mat[7]) / S;
    }
    else
    {
        S = sqrtf(1.0f + mat[8] - mat[0] - mat[4]) * 2.0f;
        qua[3] = (mat[3] - mat[1]) / S;
        qua[0] = (mat[2] + mat[6]) / S;
        qua[1] = (mat[5] + mat[7]) / S;
        qua[2] = 0.25f * S;
    }
}

void gfxMatrix4ToEuler(float* mat, float* eul)
{
    if(mat[4] > 0.998f)
    {
        eul[0] = atan2f(mat[2],mat[10])*GFX_180_DIV_PI;
        eul[1] = GFX_PI/2*GFX_180_DIV_PI;
        eul[2] = 0;
        return;
    }
    if(mat[4] < -0.998f)
    {
        eul[0] = atan2f(mat[2],mat[10])*GFX_180_DIV_PI;
        eul[1] = -GFX_PI/2*GFX_180_DIV_PI;
        eul[2] = 0;
        return;
    }
    eul[0] = atan2f(-mat[8],mat[0])*GFX_180_DIV_PI;
    eul[1] = atan2f(-mat[6],mat[5])*GFX_180_DIV_PI;
    eul[2] = asinf(mat[4])*GFX_180_DIV_PI;
}

void gfxMulMatrix4Vec3(float* m1, float* vec1, float* vec2)
{
    vec2[0] = m1[0]*vec1[0]+m1[1]*vec1[1]+m1[2]*vec1[2];
    vec2[1] = m1[4]*vec1[0]+m1[5]*vec1[1]+m1[6]*vec1[2];
    vec2[2] = m1[8]*vec1[0]+m1[9]*vec1[1]+m1[10]*vec1[2];
}

void gfxMulMatrix4Vec4(float* m1, float* vec1, float* vec2)
{
    vec2[0] = m1[0]*vec1[0]+m1[1]*vec1[1]+m1[2]*vec1[2]+m1[3]*vec1[3];
    vec2[1] = m1[4]*vec1[0]+m1[5]*vec1[1]+m1[6]*vec1[2]+m1[7]*vec1[3];
    vec2[2] = m1[8]*vec1[0]+m1[9]*vec1[1]+m1[10]*vec1[2]+m1[11]*vec1[3];
    vec2[3] = m1[12]*vec1[0]+m1[13]*vec1[1]+m1[14]*vec1[2]+m1[15]*vec1[3];
}

void gfxMulMatrix4Matrix4(float* m1, float* m2, float* m3)
{
    m3[0] = m1[0]*m2[0]+m1[1]*m2[4]+m1[2]*m2[8]+m1[3]*m2[12];
    m3[1] = m1[0]*m2[1]+m1[1]*m2[5]+m1[2]*m2[9]+m1[3]*m2[13];
    m3[2] = m1[0]*m2[2]+m1[1]*m2[6]+m1[2]*m2[10]+m1[3]*m2[14];
    m3[3] = m1[0]*m2[3]+m1[1]*m2[7]+m1[2]*m2[11]+m1[3]*m2[15];
    m3[4] = m1[4]*m2[0]+m1[5]*m2[4]+m1[6]*m2[8]+m1[7]*m2[12];
    m3[5] = m1[4]*m2[1]+m1[5]*m2[5]+m1[6]*m2[9]+m1[7]*m2[13];
    m3[6] = m1[4]*m2[2]+m1[5]*m2[6]+m1[6]*m2[10]+m1[7]*m2[14];
    m3[7] = m1[4]*m2[3]+m1[5]*m2[7]+m1[6]*m2[11]+m1[7]*m2[15];
    m3[8] = m1[8]*m2[0]+m1[9]*m2[4]+m1[10]*m2[8]+m1[11]*m2[12];
    m3[9] = m1[8]*m2[1]+m1[9]*m2[5]+m1[10]*m2[9]+m1[11]*m2[13];
    m3[10] = m1[8]*m2[2]+m1[9]*m2[6]+m1[10]*m2[10]+m1[11]*m2[14];
    m3[11] = m1[8]*m2[3]+m1[9]*m2[7]+m1[10]*m2[11]+m1[11]*m2[15];
    m3[12] = m1[12]*m2[0]+m1[13]*m2[4]+m1[14]*m2[8]+m1[15]*m2[12];
    m3[13] = m1[12]*m2[1]+m1[13]*m2[5]+m1[14]*m2[9]+m1[15]*m2[13];
    m3[14] = m1[12]*m2[2]+m1[13]*m2[6]+m1[14]*m2[10]+m1[15]*m2[14];
    m3[15] = m1[12]*m2[3]+m1[13]*m2[7]+m1[14]*m2[11]+m1[15]*m2[15];
}

void gfxMulMatrix3Matrix4(float* m1, float* m2, float* m3)
{
    m3[0] = m1[0]*m2[0]+m1[1]*m2[4]+m1[2]*m2[8];
    m3[1] = m1[0]*m2[1]+m1[1]*m2[5]+m1[2]*m2[9];
    m3[2] = m1[0]*m2[2]+m1[1]*m2[6]+m1[2]*m2[10];
    m3[3] = m1[3]*m2[0]+m1[4]*m2[4]+m1[5]*m2[8];
    m3[4] = m1[3]*m2[1]+m1[4]*m2[5]+m1[5]*m2[9];
    m3[5] = m1[3]*m2[2]+m1[4]*m2[6]+m1[5]*m2[10];
    m3[6] = m1[6]*m2[0]+m1[7]*m2[4]+m1[8]*m2[8];
    m3[7] = m1[6]*m2[1]+m1[7]*m2[5]+m1[8]*m2[9];
    m3[8] = m1[6]*m2[2]+m1[7]*m2[6]+m1[8]*m2[10];
}

unsigned char gfxBoxSphereIntersect(float* bmin, float* bmax, float* spos, float srad)
{
    float dmin;
    float r2;
    int i;

    dmin = 0;
    r2 = srad*srad;

    for(i = 0; i < 3; i++ )
    {
        if(spos[i] < bmin[i]) dmin += (spos[i]-bmin[i])*(spos[i]-bmin[i]);
        else if(spos[i] > bmax[i]) dmin += (spos[i]-bmax[i])*(spos[i]-bmax[i]);
    }

    if(dmin <= r2) return GFX_TRUE;

    return GFX_FALSE;
}

unsigned char gfxAabbInsideFrustum(float frustum[6][4], float* min, float* max)
{
    int i;

    for(i = 0; i < 6; i++)
    {
        if(frustum[i][0]*min[0]+frustum[i][1]*min[1]+frustum[i][2]*max[2]+frustum[i][3] > 0.0f)
            continue;
        if(frustum[i][0]*max[0]+frustum[i][1]*min[1]+frustum[i][2]*max[2]+frustum[i][3] > 0.0f)
            continue;
        if(frustum[i][0]*max[0]+frustum[i][1]*max[1]+frustum[i][2]*max[2]+frustum[i][3] > 0.0f)
            continue;
        if(frustum[i][0]*min[0]+frustum[i][1]*max[1]+frustum[i][2]*max[2]+frustum[i][3] > 0.0f)
            continue;
        if(frustum[i][0]*min[0]+frustum[i][1]*min[1]+frustum[i][2]*min[2]+frustum[i][3] > 0.0f)
            continue;
        if(frustum[i][0]*max[0]+frustum[i][1]*min[1]+frustum[i][2]*min[2]+frustum[i][3] > 0.0f)
            continue;
        if(frustum[i][0]*max[0]+frustum[i][1]*max[1]+frustum[i][2]*min[2]+frustum[i][3] > 0.0f)
            continue;
        if(frustum[i][0]*min[0]+frustum[i][1]*max[1]+frustum[i][2]*min[2]+frustum[i][3] > 0.0f)
            continue;
        return GFX_FALSE;
    }

    return GFX_TRUE;
}

unsigned char gfxSphereInsideFrustum(float frustum[6][4], float* pos, float radius)
{
    int i;

    for(i = 0; i < 6; i++)
    {
        if(frustum[i][0]*pos[0]+frustum[i][1]*pos[1]+
            frustum[i][2]*pos[2]+frustum[i][3] <= -radius)
            return GFX_FALSE;
    }

    return GFX_TRUE;
}

#endif
