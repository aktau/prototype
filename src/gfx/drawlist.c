/**
 * This file is part of prototype.
 *
 * (c) 2013 Nicolas Hillegeer <nicolas@hillegeer.com>
 *
 * For the full copyright and license information, please view the LICENSE
 * file that was distributed with the source code.
 */

#include "drawlist.h"
#include "util.h"
#include "binary_search.h"

#define MAX_DRAWLIST_ENTRIES 8192

/* 16 bytes (128 bits) */
struct entry {
    /* key to sort on */
    union gfxDrawlistKey key;

    /* pointer to the actual element */
    struct gfxDrawOperation *op;
};

struct gfxDrawlist {
    int dirty;

    unsigned int nextId;

    struct entry entries[MAX_DRAWLIST_ENTRIES];
};

static struct gfxDrawlist gDrawlist;

static ALWAYS_INLINE ssize_t searchEntry(uint64_t key, struct entry array[], size_t size) {
    struct entry *low = array;

    /* this should unroll... check this manually */
    for (unsigned int i = lb(size); i != 0; --i) {
        size /= 2;

        /* unconditionally unset the deleted flag, when deleting items, they
         * are not actually removed, the deleted flag is just set to 1. This
         * messes with binary search of course, as now the array is no longer
         * sorted. That's why we just "ignore" that bit by setting it back to
         * 0 in our temporary copy. */
        union gfxDrawlistKey midkey = low[size].key;
        midkey.gen.deleted = 0;

        uint64_t mid = midkey.intrep;

        if (mid <= key) {
            low += size;
        }
    }

    return (low->key.intrep > key) ? -1 : (low - array);
}

static void clearEntry(size_t index) {
    gDrawlist.entries[index].key.gen.deleted = 1;
}

/* assume a sorted list, ignore already removed items */
static ssize_t findIndex(struct gfxDrawOperation *op) {
    return searchEntry(op->key.intrep, gDrawlist.entries, MAX_DRAWLIST_ENTRIES);
}

static void printKey(union gfxDrawlistKey *k) {
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

/**
 * TODO: take care of translucency, sort translucent materials back-to-front
 */
static void sortDrawlist() {
    if (gDrawlist.dirty) {
        /* sort */

        /* find out where the last active member is
         * optimization: to start searching upwards/downwards
         * from the last position. Since the deleted flag is in the
         * most significant bit, all deleted entries will naturally
         * float to the top of the list when sorting. */
        struct entry lastmark = gDrawlist.entries[gDrawlist.nextId];
        if (lastmark.key.gen.deleted) {
            /* loop down */
            unsigned int i;
            for (i = gDrawlist.nextId; i != 0; --i) {
                if (gDrawlist.entries[i].key.gen.deleted || i == 0) {
                    gDrawlist.nextId = i;
                    break;
                }
            }
        }
        else {
            /* loop up */
            unsigned int i;
            for (i = gDrawlist.nextId; i < MAX_DRAWLIST_ENTRIES; ++i) {
                if (gDrawlist.entries[i].key.gen.deleted) {
                    gDrawlist.nextId = i;
                    break;
                }
            }

            /* check that the list is not somehow entirely filled */
            assert(i != MAX_DRAWLIST_ENTRIES - 1);
        }

        gDrawlist.dirty = 0;

        trace("drawlist was dirty, sorted\n");
    }
}

void gfxGenRenderKey(struct gfxDrawOperation *op) {
    union gfxDrawlistKey key = {0};
    op->key = key;
}

void gfxDrawlistAdd(struct gfxDrawOperation *op) {
    /* at some point we might try to re-allocate or something... */
    assert(gDrawlist.nextId < MAX_DRAWLIST_ENTRIES);

    gDrawlist.dirty = 1;

    struct entry e = {
        .key = op->key,
        .op  = op
    };

    gDrawlist.entries[gDrawlist.nextId++] = e;
}

/* remove culled objects */
void gfxDrawlistRemove(struct gfxDrawOperation *op) {
    /* assume the list is sorted */
    ssize_t idx = findIndex(op);

    if (idx != -1) {
        clearEntry((size_t) idx);
    }

    gDrawlist.dirty = 1;
}

void gfxDrawlistClear() {
    for (unsigned int i = 0; i < MAX_DRAWLIST_ENTRIES; ++i) {
        clearEntry(i);
    }

    gDrawlist.dirty  = 1;
    gDrawlist.nextId = 0;
}

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
void gfxDrawlistRender() {
    sortDrawlist();

    /* initialize local state */
    unsigned int lLayer         = 0;
    unsigned int lViewport      = 0;
    unsigned int lViewportLayer = 0;
    unsigned int lTranslucency  = 0;

    /* scan the sorted drawlist and create ad-hoc batches */
    unsigned int max = gDrawlist.nextId;
    // trace("drawing %u entities\n", max);

    for (unsigned int i = 0; i < max; ++i) {
        const struct entry e              = gDrawlist.entries[i];
        const union gfxDrawlistKey k      = e.key;
        const struct gfxDrawOperation *op = e.op;

        if (k.gen.layer != lLayer) {
            lLayer = k.gen.layer;
        }

        if (k.gen.viewport != lViewport) {
            lViewport = k.gen.viewport;
        }

        if (k.gen.viewportLayer != lViewportLayer) {
            lViewportLayer = k.gen.viewportLayer;
        }

        if (k.gen.translucency != lTranslucency) {
            lTranslucency = k.gen.translucency;
        }

        /* TODO: don't do this unnecesarily */
        gfxBatch(op->params);

        glUseProgram(op->program->id);
        glBindVertexArray(op->model->vao);

        if (op->model->texture[0]) {
            glActiveTexture(GL_TEXTURE0 + 0);
            glBindTexture(GL_TEXTURE_2D, op->model->texture[0]);
        }

        gfxSetShaderParams(op->program, op->params);

        /* fire draw batch */
        glDrawElements(GL_TRIANGLES, op->model->numIndices, GL_UNSIGNED_BYTE, (GLvoid*)0);
    }

    glBindVertexArray(0);
    glUseProgram(0);
}

void gfxDrawlistDebug() {
    STATIC_ASSERT(sizeof(union gfxDrawlistKey) == sizeof(uint64_t), "the largest item in the struct must be as large as the integer representation");

    trace("size of a key struct: %lu, size of an entry struct: %lu (gen: %lu, cmd: %lu, model: %lu)\n",
        sizeof(union gfxDrawlistKey), sizeof(struct entry), sizeof(struct gfxDrawOpGeneric), sizeof(struct gfxDrawOpCommand), sizeof(struct gfxDrawOpEntity));

    struct gfxDrawOpEntity mod  = {0};
    struct gfxDrawOpCommand cmd = {0};
    union gfxDrawlistKey tk     = {0};

    /* generic attributes */
    mod.layer         = 0;
    mod.viewport      = 1;
    mod.viewportLayer = 2;
    mod.translucency  = 1;
    mod.type          = KEY_TYPE_MODEL;

    /* model specific attributes */
    mod.depth    = 15000;
    mod.material = 5;

    tk.mod = mod;

    printKey(&tk);


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
