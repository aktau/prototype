/**
 * This file is part of prototype.
 *
 * (c) 2013 Nicolas Hillegeer <nicolas@hillegeer.com>
 *
 * For the full copyright and license information, please view the LICENSE
 * file that was distributed with the source code.
 */

#ifndef __drawlist_h__
#define __drawlist_h__

#include <stdint.h>

#include "macros.h"

typedef enum {
    LAYER_0 = 0,
    LAYER_1,
    LAYER_2,
    LAYER_3,
} layer_t;

typedef enum {
    VIEWPORT_0 = 0,
    VIEWPORT_1,
    VIEWPORT_2,
    VIEWPORT_3,
} viewport_t;

typedef enum {
    VIEWPORT_LAYER_0 = 0,
    VIEWPORT_LAYER_1,
    VIEWPORT_LAYER_2,
    VIEWPORT_LAYER_3,
} viewport_layer_t;

/* opaque objects should get drawn first */
typedef enum {
    OPAQUE = 0,
    NORMAL,
    ADDITIVE,
    SUBTRACTIVE,
} translucency_t;

typedef enum {
    KEY_TYPE_MODEL   = 0,
    KEY_TYPE_COMMAND = 1
} keytype_t;

/* 2 + 2 + 2 + 2 + 1 + 6 + 1 = 16 bits */
#define GENERIC_FIELDS \
 unsigned int type : 1; \
 unsigned int translucency : 2; \
 unsigned int viewportLayer : 2; \
 unsigned int viewport : 2; \
 unsigned int layer : 2; \
 unsigned int : 6; \
 unsigned int deleted: 1;

/* 8 + 32 + 8 = 48 bits */
#define COMMAND_FIELDS \
 unsigned int : 8; \
 unsigned int id : 32; \
 unsigned int sequence : 8;

/* 16 + 16 + 8 + 8 = 48 bits */
#define MODEL_FIELDS \
 unsigned int material : 8; \
 unsigned int depth : 16; \
 unsigned int shader : 8; \
 unsigned int texture : 8; \
 unsigned int model : 8;

/* 48 bits, padding */
#define EMPTY_FIELDS \
 unsigned int : 32; \
 unsigned int : 16;

struct gfxDrawOpGeneric {
    EMPTY_FIELDS
    GENERIC_FIELDS
} PACKED;

struct gfxDrawOpCommand {
    COMMAND_FIELDS
    GENERIC_FIELDS
} PACKED;

struct gfxDrawOpEntity {
    MODEL_FIELDS
    GENERIC_FIELDS
} PACKED;

union gfxDrawlistKey {
    uint64_t intrep;
    struct gfxDrawOpGeneric gen;
    struct gfxDrawOpCommand cmd;
    struct gfxDrawOpEntity mod;
};

struct gfxDrawOperation;

void gfxGenRenderKey(struct gfxDrawOperation *op);

void gfxDrawlistAdd(struct gfxDrawOperation *op);
void gfxDrawlistClear();
void gfxDrawlistRemove(struct gfxDrawOperation *op);
void gfxDrawlistRender();
void gfxDrawlistDebug();

/**
 * alternatively, we could use defines and macros...
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

#endif
