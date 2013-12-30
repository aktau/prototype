/**
 * This file is part of prototype.
 *
 * (c) 2013 Nicolas Hillegeer <nicolas@hillegeer.com>
 *
 * For the full copyright and license information, please view the LICENSE
 * file that was distributed with the source code.
 */

#include "util.h"

/* 2 + 3 + 3 + 2 + 1 + 5 = 16 bits */
#define GENERIC_FIELDS \
 unsigned int layer : 2; \
 unsigned int viewport : 3; \
 unsigned int viewportLayer : 3; \
 unsigned int translucency : 2; \
 unsigned int type : 1; \
 unsigned int : 5;

enum {
    KEY_TYPE_MODEL   = 0,
    KEY_TYPE_COMMAND = 1
} keytype_t;

/* 8 + 32 + 8 = 48 bits */
#define COMMAND_FIELDS \
 unsigned int sequence : 8; \
 unsigned int id : 32; \
 unsigned int : 8;

/* 24 + 24 = 48 bits */
#define MODEL_FIELDS \
 unsigned int depth : 24; \
 unsigned int material : 24;

struct generic {
    /* 16 bits */
    GENERIC_FIELDS
};

struct command {
    /* 16 bits */
    GENERIC_FIELDS

    /* 48 bits */
    COMMAND_FIELDS
} PACKED;

struct model {
    /* 16 bits */
    GENERIC_FIELDS

    /* 48 bits */
    MODEL_FIELDS
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
void gfxDrawlistRemove() {}

/* renders the current drawlist */
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
    trace("the integer representation is %llu\n", k->intrep);
}

void gfxDrawlistDebug() {
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
}
