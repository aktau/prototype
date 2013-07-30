/**
 * This file is part of prototype.
 *
 * (c) 2013 Nicolas Hillegeer <nicolas@hillegeer.com>
 *
 * For the full copyright and license information, please view the LICENSE
 * file that was distributed with the source code.
 */

#include <math.h>

inline void cross(float *restrict a, float *restrict b, float *restrict res) {
    res[0] = a[1] * b[2] - b[1] * a[2];
    res[1] = a[2] * b[0] - b[2] * a[0];
    res[2] = a[0] * b[1] - b[0] * a[1];
}

inline void normalize(float *a) {
    float mag = sqrtf(a[0]*a[0] + a[1]*a[1] + a[2]*a[2]);

    a[0] /= mag;
    a[1] /= mag;
    a[2] /= mag;
}
