/**
 * This file is part of prototype.
 *
 * (c) 2013 Nicolas Hillegeer <nicolas@hillegeer.com>
 *
 * For the full copyright and license information, please view the LICENSE
 * file that was distributed with the source code.
 */

#include "util.h"

/* provides performance monitoring primitives through OpenGL queries.
 * TODO: perhaps also make an interface to glQueryCounter? Which can equally
 * well be used (and be more flexible) in some cases. reference:
 * http://www.lighthouse3d.com/tutorials/opengl-short-tutorials/opengl-timer-query
 * glQueryCounter can be mixed recursively. */

/* don't want to to think too hard about the right memory model for the
 * atomic ops right now and it's not critical so I'll go for the safest one. */
#define PERF_MEMORY_MODEL __ATOMIC_SEQ_CST

static int gfxFramesLatency(struct gfxQuerySet *set, gfx_query_t type, unsigned int curr);
static unsigned int gfxPerfIndex(unsigned int counter);

/* generates GFX_QUERY_FRAMES sets of query objects, so we never have to stall */
void gfxGenQueries(struct gfxQuerySet *set) {
  trace("generating %d frames of GL queries...\n", GFX_QUERY_FRAMES);

  memset(set, 0, sizeof(*set));

  for (int i = 0; i < GFX_QUERY_FRAMES; ++i) {
    glGenQueries(GFX_TIMER_NUM, set->queries[i]);
  }
  GL_ERROR("generate timer queries");

  /* perform one fake frame to prevent GL errors */
  for (int i = 0; i < GFX_TIMER_NUM; ++i) {
    /* TODO: I really need to find a better solution for this than hardcoding
         * the type of the query... */
    GLenum type;
    switch (i) {
    case GFX_TIMER_RENDER:
    case GFX_TIMER_SWAP:
      type = GL_TIME_ELAPSED;
      break;

    case GFX_PRIMITIVES_GENERATED:
      type = GL_PRIMITIVES_GENERATED;
      break;
    }

    glBeginQuery(type, set->queries[0][i]);
    GL_ERROR("dummy init: %d", i);

    glEndQuery(type);
    GL_ERROR("dummy read: %d", i);
  }

  set->curr = 0;
}

void gfxDestroyQueries(struct gfxQuerySet *set) {
  for (int i = 0; i < GFX_QUERY_FRAMES; ++i) {
    glDeleteQueries(GFX_TIMER_NUM, set->queries[i]);
  }
  GL_ERROR("delete timer queries");
}

/* wrapper around glBeginQuery that automatically targets the correct frame.
 * Don't nest calls to the same target (GL_TIME_ELAPSED, ...). (if we want
 * to do that for regular timers we need glQueryCounter. */
void gfxBeginQuery(struct gfxQuerySet *set, GLenum target, gfx_query_t type) {
  unsigned int curr = __atomic_load_n(&set->curr, PERF_MEMORY_MODEL);
  glBeginQuery(target, set->queries[gfxPerfIndex(curr)][type]);
}

/* wrapper around glEndQuery that automatically targets the correct frame.
 * Ends the currently running query of the given type. */
void gfxEndQuery(struct gfxQuerySet *set, GLenum target) {
  /* for now we're not doing anything with the gfxQuerySet argument but we
     * might in the future, for example for sanity checking (we can check
     * recursive use of timers... */
  glEndQuery(target);
}

/* switches to the next set of query buffers, this has to be done after
 * every frame. */
void gfxPerfFinishFrame(struct gfxQuerySet *set) {
  /* the gQueries array works like a ring buffer, set->curr is incremented
     * until it reaches the end of the array (GFX_QUERY_FRAMES). Don't check
     * for overflow because I'm going to assume we're on a two's complement
     * architecture were overflow acts like module wraparound. */

  /* __atomic_fetch_add is like set->curr++ (postfix) */
  unsigned int curr = __atomic_fetch_add(&set->curr, 1, PERF_MEMORY_MODEL);

#ifdef GFX_PERF_NFRAMES_LATENCY
  int nframes = gfxFramesLatency(set, GFX_TIMER_RENDER, curr);
  if (nframes != -1) {
    set->numFramesLatency = nframes;
  }
#endif
}

#define VALUE_OR_BAIL(value, set, func)                                   \
  do {                                                                    \
    unsigned int curr = __atomic_load_n(&set->curr, PERF_MEMORY_MODEL);   \
    int nframes = gfxFramesLatency(set, type, curr);                      \
    if (nframes == -1) return 0;                                          \
    unsigned int firstframe = gfxPerfIndex(curr - (unsigned int)nframes); \
    func(set->queries[firstframe][type], GL_QUERY_RESULT, &(value));      \
  } while (0);

uint32_t gfxPerfGetu32(struct gfxQuerySet *set, gfx_query_t type) {
  STATIC_ASSERT(sizeof(uint32_t) == sizeof(GLuint), "sizeof(uint32_t) != sizeof(GLuint)");

  uint32_t val;
  VALUE_OR_BAIL(val, set, glGetQueryObjectuiv);
  return val;
}

/* get the first available value of this kind */
uint64_t gfxPerfGetu64(struct gfxQuerySet *set, gfx_query_t type) {
  STATIC_ASSERT(sizeof(uint64_t) == sizeof(GLuint64), "sizeof(uint64_t) != sizeof(GLuint64)");

  uint64_t val;
  VALUE_OR_BAIL(val, set, glGetQueryObjectui64v);
  return val;
}

/* returns -1 if there's not enough information to calculate the latency */
static int gfxFramesLatency(struct gfxQuerySet *set, gfx_query_t type, unsigned int curr) {
  if (curr < GFX_QUERY_FRAMES - 1) {
    /* if we haven't done enough frames yet, just return, don't want to
         * deal with boundary conditions. */
    return -1;
  }

  int counter = 0;
  unsigned int earliest = curr - (GFX_QUERY_FRAMES - 1);
  for (unsigned int i = curr; i >= earliest; --i) {
    unsigned int frame = gfxPerfIndex(i);

    GLint done = 0;
    glGetQueryObjectiv(set->queries[frame][type],
                       GL_QUERY_RESULT_AVAILABLE, &done);

    /* if the result is available, stop the loop, we've reached the
         * number of frames that the GPU is "behind". */
    if (done) {
      return counter;
    }

    /* increase the latency counter */
    ++counter;
  }

  return -1;
}

/* calculates the index in the ring buffer based on the position of the
 * increasing counter */
static unsigned int gfxPerfIndex(unsigned int counter) {
  return counter % GFX_QUERY_FRAMES;
}
