/**
 * This file is part of prototype.
 *
 * (c) 2013 Nicolas Hillegeer <nicolas@hillegeer.com>
 *
 * For the full copyright and license information, please view the LICENSE
 * file that was distributed with the source code.
 */

#include "util.h"

/* alternatively, we could use defines and macros...
 * which would surely be way more portable, though also
 * uglier... will do when we port stuff.
 *
 * example:
 *
 * typedef key_t uint64_t;
 *
 * #define TOTAL_BITS sizeof(key_t) * 8
 *
 * #define LAYER_BITS 2
 * #define LAYER_OFFSET TOTAL_BITS - LAYER_BITS
 *
 * #define VIEWPORT_BITS 3
 * #define VIEWPORT_OFFSET LAYER_OFFSET - VIEWPORT_BITS
 * ...
 */

/* 2 + 3 + 3 + 2 + 1 + 5 = 16 bits */
#define GENERIC_FIELDS \
 unsigned int : 5; \
 unsigned int type : 1; \
 unsigned int translucency : 2; \
 unsigned int viewportLayer : 3; \
 unsigned int viewport : 3; \
 unsigned int layer : 2;

enum {
    KEY_TYPE_MODEL   = 0,
    KEY_TYPE_COMMAND = 1
} keytype_t;

/* 8 + 32 + 8 = 48 bits */
#define COMMAND_FIELDS \
 unsigned int : 8; \
 unsigned int id : 32; \
 unsigned int sequence : 8;

/* 24 + 24 = 48 bits */
#define MODEL_FIELDS \
 unsigned int material : 24; \
 unsigned int depth : 24;

/* 48 bits, padding */
#define EMPTY_FIELDS \
 unsigned int : 32; \
 unsigned int : 16;

struct generic {
    EMPTY_FIELDS /* 48 bits */
    GENERIC_FIELDS /* 16 bits */
};

struct command {
    COMMAND_FIELDS /* 48 bits */
    GENERIC_FIELDS /* 16 bits */
} PACKED;

struct model {
    MODEL_FIELDS /* 48 bits */
    GENERIC_FIELDS /* 16 bits */
} PACKED;

union key {
    uint64_t intrep;
    struct generic gen;
    struct command cmd;
    struct model mod;
};

/* 16 bytes (128 bits) */
struct entry {
    /* key to sort on */
    union key key;

    /* pointer to the actual element */
    int *p;
};

void gfxDrawlistAdd() {}

/* remove culled objects */
void gfxDrawlistRemove() {

}

static void gfxDrawlistSort() {}

/* renders the current drawlist
 * some things to do:
 *
 * resources:
 * 2007 - very comprehensive (GPU Gems) - https://developer.nvidia.com/content/gpu-gems-chapter-28-graphics-pipeline-performance
 *
 * general:
 * - do anything to increase batch size. The amount of batches/second are limited,
 *   yet the amount of tris/batch is extremely high, and rising. Common ways to batch are
 *   (hardware) instancing, transform matrix/weight arrays, stitching VBO's together, not using
 *   shader constants (uniforms) which you have to set per-model or group of models, but encode
 *   data in (for example) the alpha channel of the model texture. This loses flexibility, but can
 *   increase performance a lot. When there's few/no state changes between draw calls (depending on
 *   the type of state changes), the GPU can batch these draw calls, making the model almost
 *   free to render thanks to the massively parallel architecture of the GPU.
 *
 * pre-process:
 * - pack multiple textures into one texture (requires tool support)
 * - make an optional runtime decision:
 *   1) pass normal, binormal, tangent
 *   -or-
 *   2) pass normal, binormal (or tangent), calculate the third in the vert shader
 *   this trades vertex transfer for vertex processing. You can see at runtime which is the limiting factor
 * - indexed primitives seem to be preferred (with max 16-bit indices), they are able
 *   to benefit from the post-T&L cache if ordered correctly
 *
 * process (per frame):
 * - put transform matrices into a uniform array or something similar so the batch
 *   doesn't have to be broken. This requires support from the shader though...
 * - encode world matrix as 2 float4's (batch batch batch - axis/angle, translate/uniform scale - still relevant?)
 * - use all texture units (batch batch batch)
 * - transform coordinate bases once per frame: Frequently, choice of coordinate space affects the number of instructions
 *   required to compute a value in the vertex program. For example, when doing vertex lighting, if your vertex normals
 *   are stored in object space and the light vector is stored in eye space, then you will have to transform one of the
 *   two vectors in the vertex shader. If the light vector was instead transformed into object space once per object on
 *   the CPU, no per-vertex transformation would be necessary, saving GPU vertex instructions.
 * - primitives share material: can stitch together?
 * - primitives are the same: hardware/pseudo-instancing */
void gfxDrawlistRender() {}

static void printKey(union key *k) {
    trace("the generic fields are:\n\tlayer = %u\n\tviewport = %u\n\tviewportlayer = %u\n\ttranslucency = %u\n\ttype = %u\n",
        k->gen.layer,
        k->gen.viewport,
        k->gen.viewportLayer,
        k->gen.translucency,
        k->gen.type);
    switch (k->gen.type) {
        case KEY_TYPE_MODEL:
            trace("the specific fields are:\n\tdepth = %u\n\tmaterial = %u\n",
                k->mod.depth,
                k->mod.material);
        break;
        case KEY_TYPE_COMMAND:
            trace("the specific fields are:\n\tsequence = %u\n\tid = %u\n",
                k->cmd.sequence,
                k->cmd.id);
        break;
    }
    trace("the integer representation is %llu (0x%016llX)\n", k->intrep, k->intrep);
}

void gfxDrawlistDebug() {
    STATIC_ASSERT(sizeof(union key) == sizeof(uint64_t), "the largest item in the struct must be as large as the integer representation");

    trace("size of a key struct: %lu, size of an entry struct: %lu (gen: %lu, cmd: %lu, model: %lu)\n",
        sizeof(union key), sizeof(struct entry), sizeof(struct generic), sizeof(struct command), sizeof(struct model));

    struct model mod = {0};

    /* generic attributes */
    mod.layer         = 0;
    mod.viewport      = 1;
    mod.viewportLayer = 2;
    mod.translucency  = 1;
    mod.type          = KEY_TYPE_MODEL;

    /* model specific attributes */
    mod.depth    = 15000;
    mod.material = 5;

    union key tk = {0};
    tk.mod = mod;

    printKey(&tk);

    struct command cmd = {0};

    cmd.layer         = 0;
    cmd.viewport      = 0;
    cmd.viewportLayer = 0;
    cmd.translucency  = 0;
    cmd.type          = KEY_TYPE_COMMAND;

    cmd.sequence = 0;
    cmd.id       = 0;

    tk.cmd = cmd;

    printKey(&tk);

    mod.layer         = 0;
    mod.viewport      = 0;
    mod.viewportLayer = 0;
    mod.translucency  = 0;
    mod.type          = KEY_TYPE_MODEL;

    mod.depth    = 0;
    mod.material = 1;

    tk.mod = mod;

    printKey(&tk);

    mod.depth    = 1;
    mod.material = 0;

    tk.mod = mod;

    printKey(&tk);

    mod.layer         = 0;
    mod.viewport      = 1;
    mod.viewportLayer = 0;
    mod.translucency  = 0;
    mod.type          = KEY_TYPE_MODEL;

    mod.depth    = 0;
    mod.material = 0;

    tk.mod = mod;

    printKey(&tk);

    mod.layer         = 0;
    mod.viewport      = 0;
    mod.viewportLayer = 1;
    mod.translucency  = 0;
    mod.type          = KEY_TYPE_MODEL;

    mod.depth    = 0;
    mod.material = 0;

    tk.mod = mod;

    printKey(&tk);

    mod.layer         = 0;
    mod.viewport      = 0;
    mod.viewportLayer = 0;
    mod.translucency  = 1;
    mod.type          = KEY_TYPE_MODEL;

    mod.depth    = 0;
    mod.material = 0;

    tk.mod = mod;

    printKey(&tk);

    cmd.layer         = 0;
    cmd.viewport      = 0;
    cmd.viewportLayer = 0;
    cmd.translucency  = 0;
    cmd.type          = KEY_TYPE_COMMAND;

    cmd.sequence = 1;
    cmd.id       = 1;

    tk.cmd = cmd;

    printKey(&tk);

    cmd.layer         = 0;
    cmd.viewport      = 0;
    cmd.viewportLayer = 0;
    cmd.translucency  = 0;
    cmd.type          = KEY_TYPE_COMMAND;

    cmd.sequence = 1;
    cmd.id       = 0;

    tk.cmd = cmd;

    printKey(&tk);
}
