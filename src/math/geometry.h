/**
 * This file is part of prototype.
 *
 * (c) 2013 Iñigo Iquilez <http://www.iquilezles.org/>
 * (c) 2014 Nicolas Hillegeer <nicolas@hillegeer.com>
 *
 * For the full copyright and license information, please view the LICENSE
 * file that was distributed with the source code.
 */

#ifndef THREEDEE_GEOMETRY_H
#define THREEDEE_GEOMETRY_H

#include <math/macros.h>
#include <math/vector.h>

/* this will (hopefully) always be run in a tight loop on adjacent memory, so inline it
 * originally written by Iñigo Iquilez */
ALWAYS_INLINE static int box_in_frustum_scalar(const frustum fru, const aabb box) {
    /* check box outside/inside of frustum */
    for (int i = 0; i < 6; ++i) {
        int out = 0;
        out += (vdot(fru.planes[i], vec(box.min[0], box.min[1], box.min[2], 1.0f))[0] < 0.0f) ? 1 : 0;
        out += (vdot(fru.planes[i], vec(box.max[0], box.min[1], box.min[2], 1.0f))[0] < 0.0f) ? 1 : 0;
        out += (vdot(fru.planes[i], vec(box.min[0], box.max[1], box.min[2], 1.0f))[0] < 0.0f) ? 1 : 0;
        out += (vdot(fru.planes[i], vec(box.max[0], box.max[1], box.min[2], 1.0f))[0] < 0.0f) ? 1 : 0;
        out += (vdot(fru.planes[i], vec(box.min[0], box.min[1], box.max[2], 1.0f))[0] < 0.0f) ? 1 : 0;
        out += (vdot(fru.planes[i], vec(box.max[0], box.min[1], box.max[2], 1.0f))[0] < 0.0f) ? 1 : 0;
        out += (vdot(fru.planes[i], vec(box.min[0], box.max[1], box.max[2], 1.0f))[0] < 0.0f) ? 1 : 0;
        out += (vdot(fru.planes[i], vec(box.max[0], box.max[1], box.max[2], 1.0f))[0] < 0.0f) ? 1 : 0;

        /*
        out += (vdot(fru.planes[i], box.min) < zero) ? 1 : 0;
        out += (vdot(fru.planes[i], vadvshuffle(box.max, box.min, 0, 5, 6, 7)) < zero) ? 1 : 0;
        out += (vdot(fru.planes[i], vadvshuffle(box.max, box.min, 4, 1, 6, 7)) < zero) ? 1 : 0;
        out += (vdot(fru.planes[i], vadvshuffle(box.max, box.min, 0, 1, 6, 7)) < zero) ? 1 : 0;
        out += (vdot(fru.planes[i], vadvshuffle(box.max, box.min, 4, 5, 2, 3)) < zero) ? 1 : 0;
        out += (vdot(fru.planes[i], vadvshuffle(box.max, box.min, 0, 5, 2, 3)) < zero) ? 1 : 0;
        out += (vdot(fru.planes[i], vadvshuffle(box.max, box.min, 4, 1, 2, 3)) < zero) ? 1 : 0;
        out += (vdot(fru.planes[i], box.max) < zero) ? 1 : 0;
        */

        if (out == 8) return 0;
    }

    /* check frustum outside/inside box */
    int out;
    out = 0; for (int i = 0; i < 8; ++i) out += ((fru.points[i][0] > box.max[0]) ? 1 : 0); if (out == 8) return 0;
    out = 0; for (int i = 0; i < 8; ++i) out += ((fru.points[i][0] < box.min[0]) ? 1 : 0); if (out == 8) return 0;
    out = 0; for (int i = 0; i < 8; ++i) out += ((fru.points[i][1] > box.max[1]) ? 1 : 0); if (out == 8) return 0;
    out = 0; for (int i = 0; i < 8; ++i) out += ((fru.points[i][1] < box.min[1]) ? 1 : 0); if (out == 8) return 0;
    out = 0; for (int i = 0; i < 8; ++i) out += ((fru.points[i][2] > box.max[2]) ? 1 : 0); if (out == 8) return 0;
    out = 0; for (int i = 0; i < 8; ++i) out += ((fru.points[i][2] < box.min[2]) ? 1 : 0); if (out == 8) return 0;

    return 1;
}

ALWAYS_INLINE static void transform_points_4(vec4 result[3], vec4 x, vec4 y, vec4 z, mat4 transform) {
#define COMP(c) \
    vec4 res_ ## c; \
    res_ ## c = vsplat(transform.cols[3], c); \
    res_ ## c = vmadd(z, vsplat(transform.cols[2], c), res_ ## c); \
    res_ ## c = vmadd(y, vsplat(transform.cols[1], c), res_ ## c); \
    res_ ## c = vmadd(x, vsplat(transform.cols[0], c), res_ ## c); \
    result[c] = res_ ## c;

    COMP(0);
    COMP(1);
    COMP(2);
#undef COMP
}

ALWAYS_INLINE static vec4 dot4(vec4 v, vec4 x, vec4 y, vec4 z) {
    vec4 result;

    result = vsplat(v, 3);
    result = vmadd(vsplat(v, 2), z, result);
    result = vmadd(vsplat(v, 1), y, result);
    result = vmadd(vsplat(v, 0), x, result);

    return result;
}

/* current impl based on: http://zeuxcg.org/2009/02/15/view-frustum-culling-optimization-%E2%80%93-structures-and-arrays/
 * and needs testing. Further optimizations possible with extent testing and suchlike, but we'll need another function
 * definition. The calling function will have to precalculate some stuff. */
ALWAYS_INLINE static int box_in_frustum_soa(const frustum fru, const aabb box) {
    /**
     * Create a struct-of-arrays style representation of the aabb
     *
     * = [x X x X]
     * = [y y Y Y]
     * = [z z z z]
     * = [Z Z Z Z]
     */
    const vec4 minmax_x   = vadvshuffle(box.min, box.max, 0, 4, 0, 4);
    const vec4 minmax_y   = vadvshuffle(box.min, box.max, 1, 1, 5, 5);
    const vec4 minmax_z_0 = vsplat(box.min, 2);
    const vec4 minmax_z_1 = vsplat(box.max, 2);

    /**
     * transform points to world-space
     */
    mat4 transform = midentity();
    vec4 tx1[3], tx2[3];
    transform_points_4(tx1, minmax_x, minmax_y, minmax_z_0, transform);
    transform_points_4(tx2, minmax_x, minmax_y, minmax_z_1, transform);

    for (int i = 0; i < 6; ++i) {
        /* TODO: possibly test for early out after first dot-product series */

        /* calculate 8 dot products, note to self, check if
         * compiler actually eliminates redundant splats...
         * a negative value implies that the point is outside
         * of the frustum plane */
        vec4 dp0 = dot4(fru.planes[i], tx1[0], tx1[1], tx1[2]);
        vec4 dp1 = dot4(fru.planes[i], tx2[0], tx2[1], tx2[2]);

        /* get signs */
        int mask = vmovmask(vand(dp0, dp1));

        /* if all points are out, quit early */
        if (mask == 0x0F) return 0;
    }

    /* TODO: vectorize */
    /* check frustum outside/inside box */
    int out;
    out = 0; for (int i = 0; i < 8; ++i) out += ((fru.points[i][0] > box.max[0]) ? 1 : 0); if (out == 8) return 0;
    out = 0; for (int i = 0; i < 8; ++i) out += ((fru.points[i][0] < box.min[0]) ? 1 : 0); if (out == 8) return 0;
    out = 0; for (int i = 0; i < 8; ++i) out += ((fru.points[i][1] > box.max[1]) ? 1 : 0); if (out == 8) return 0;
    out = 0; for (int i = 0; i < 8; ++i) out += ((fru.points[i][1] < box.min[1]) ? 1 : 0); if (out == 8) return 0;
    out = 0; for (int i = 0; i < 8; ++i) out += ((fru.points[i][2] > box.max[2]) ? 1 : 0); if (out == 8) return 0;
    out = 0; for (int i = 0; i < 8; ++i) out += ((fru.points[i][2] < box.min[2]) ? 1 : 0); if (out == 8) return 0;

    return 1;
}

ALWAYS_INLINE static int box_in_frustum_soa_early(const frustum fru, const aabb box) {
    /**
     * Create a struct-of-arrays style representation of the aabb
     *
     * = [x X x X]
     * = [y y Y Y]
     * = [z z z z]
     * = [Z Z Z Z]
     */
    const vec4 minmax_x   = vadvshuffle(box.min, box.max, 0, 4, 0, 4);
    const vec4 minmax_y   = vadvshuffle(box.min, box.max, 1, 1, 5, 5);
    const vec4 minmax_z_0 = vsplat(box.min, 2);
    const vec4 minmax_z_1 = vsplat(box.max, 2);

    /* transform points to world-space */
    mat4 transform = midentity();
    vec4 tx1[3], tx2[3];
    transform_points_4(tx1, minmax_x, minmax_y, minmax_z_0, transform);
    transform_points_4(tx2, minmax_x, minmax_y, minmax_z_1, transform);

    for (int i = 0; i < 6; ++i) {
        /* first 4 dot products, early continue if at least one of them is positive */
        vec4 dp0 = dot4(fru.planes[i], tx1[0], tx1[1], tx1[2]);
        if (vmovmask(dp0) != 0x0F) continue;

        vec4 dp1 = dot4(fru.planes[i], tx2[0], tx2[1], tx2[2]);
        if (vmovmask(dp1) == 0x0F) return 0;
    }

    /* TODO: vectorize */
    /* check frustum outside/inside box */
    int out;
    out = 0; for (int i = 0; i < 8; ++i) out += ((fru.points[i][0] > box.max[0]) ? 1 : 0); if (out == 8) return 0;
    out = 0; for (int i = 0; i < 8; ++i) out += ((fru.points[i][0] < box.min[0]) ? 1 : 0); if (out == 8) return 0;
    out = 0; for (int i = 0; i < 8; ++i) out += ((fru.points[i][1] > box.max[1]) ? 1 : 0); if (out == 8) return 0;
    out = 0; for (int i = 0; i < 8; ++i) out += ((fru.points[i][1] < box.min[1]) ? 1 : 0); if (out == 8) return 0;
    out = 0; for (int i = 0; i < 8; ++i) out += ((fru.points[i][2] > box.max[2]) ? 1 : 0); if (out == 8) return 0;
    out = 0; for (int i = 0; i < 8; ++i) out += ((fru.points[i][2] < box.min[2]) ? 1 : 0); if (out == 8) return 0;

    return 1;
}

ALWAYS_INLINE static int box_in_frustum(const frustum fru, const aabb box) {
    return box_in_frustum_scalar(fru, box);
}

#endif
