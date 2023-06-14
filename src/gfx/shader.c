/**
 * This file is part of prototype.
 *
 * (c) 2013 Nicolas Hillegeer <nicolas@hillegeer.com>
 *
 * For the full copyright and license information, please view the LICENSE
 * file that was distributed with the source code.
 */

#include "util.h"

#define GL_SHADER_ERROR(shader)     \
  if (DEBUG_TEST) {                 \
    assert(gfxCheckShader(shader)); \
  }

#define GL_PROGRAM_ERROR(program)           \
  if (DEBUG_TEST) {                         \
    assert(gfxCheckShaderProgram(program)); \
  }

static int gfxCheckShader(GLuint shader);
static int gfxCheckShaderProgram(GLuint program);
static size_t gfxGlslTypeSize(GLint type);
static const char *gfxGlslTypeString(GLint type);
static int gfxUboInfoCompare(const void *p1, const void *p2);

void gfxSetShaderParams(
    const struct gfxShaderProgram *shader,
    const struct gfxLayer *layer,
    const struct gfxRenderParams *params,
    const struct gfxRenderParams *prev) {
  /* TODO: glEnable(GL_DEPTH_TEST); */

  /* blending: http://stackoverflow.com/questions/6853004/procedure-for-alpha-blending-textured-quads-in-opengl-3 */
  if (!prev || params->blend != prev->blend) {
    if (params->blend == GFX_NONE) {
      glDisable(GL_BLEND);
    } else {
      glEnable(GL_BLEND);

      switch (params->blend) {
      case GFX_BLEND_ALPHA:
        /* (almost) equivalent to the following
         *
         * glBlendEquation(GL_FUNC_ADD);
         * glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
         *
         * except that the alpha channel itself gets blended differently */
        glBlendEquationSeparate(GL_FUNC_ADD, GL_FUNC_ADD);
        glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ZERO);
        break;
      case GFX_BLEND_PREMUL_ALPHA:
        glBlendEquationSeparate(GL_FUNC_ADD, GL_FUNC_ADD);
        glBlendFuncSeparate(GL_ONE, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ZERO);
        break;
      }
    }
  }

  /* culling */
  if (!prev || params->cull != prev->cull) {
    if (params->cull == GFX_NONE) {
      glDisable(GL_CULL_FACE);
    } else {
      glEnable(GL_CULL_FACE);
      glFrontFace(GL_CCW);

      switch (params->cull) {
      case GFX_CULL_FRONT:
        glCullFace(GL_FRONT);
        break;
      case GFX_CULL_BACK:
        glCullFace(GL_BACK);
        break;
      }
    }
  }

  if (shader->loc.modelviewMatrix != -1) glUniformMatrix4fv(shader->loc.modelviewMatrix, 1, GL_FALSE, (const GLfloat *)&params->modelviewMatrix);

  /* TODO: textures are possibly saved in the program state, don't need to call glUniform() all the time... */
  if (shader->loc.texture0 != -1) glUniform1i(shader->loc.texture0, 0);
  if (shader->loc.texture1 != -1) glUniform1i(shader->loc.texture1, 1);
  if (shader->loc.texture2 != -1) glUniform1i(shader->loc.texture2, 2);
  if (shader->loc.texture3 != -1) glUniform1i(shader->loc.texture3, 3);

  if (shader->loc.timer != -1) glUniform1f(shader->loc.timer, layer->uniforms.timer);
}

void gfxLoadShaderFromFile(struct gfxShaderProgram *shader, const char *vertfile, const char *fragfile) {
  GLchar *const vertsrc = (GLchar *const)loadfile(vertfile);
  GLchar *const fragsrc = (GLchar *const)loadfile(fragfile);

  gfxLoadShader(shader, vertsrc, fragsrc);

  zfree(vertsrc);
  zfree(fragsrc);
}

void gfxLoadShader(struct gfxShaderProgram *shader, const char *vertsrc, const char *fragsrc) {
  memset(shader, 0x0, sizeof(struct gfxShaderProgram));

  trace("vertex shader: \n%s\n", vertsrc);
  trace("fragment shader: \n%s\n", fragsrc);

  /* create and compile shaders */
  GLuint vert = glCreateShader(GL_VERTEX_SHADER);
  GLuint frag = glCreateShader(GL_FRAGMENT_SHADER);

  glShaderSource(vert, 1, &vertsrc, NULL);
  glShaderSource(frag, 1, &fragsrc, NULL);

  glCompileShader(vert);

  GL_SHADER_ERROR(vert);
  GL_ERROR("create and compile vertex shader");

  glCompileShader(frag);

  GL_SHADER_ERROR(frag);
  GL_ERROR("create and compile fragment shader");

  /* create and link program */
  GLuint program = glCreateProgram();

  glAttachShader(program, vert);
  glAttachShader(program, frag);

  GL_ERROR("attach shaders");

  /* We're specifying the layout in the shaders now (this OpenGL 3.3
   * feature is thankfully supported on OSX 10.7+). So now you can
   * name your vertex attributes what you want. Be aware that the
   * engine will always choose the same order though, as detailed
   * below in the "legacy" code:
   *
   * glBindAttribLocation(program, GFX_VERTEX, "in_position");
   * glBindAttribLocation(program, GFX_NORMAL, "in_normal");
   * glBindAttribLocation(program, GFX_TEXCOORD, "in_texcoord");
   * glBindAttribLocation(program, GFX_COLOR, "in_color");
   * glBindAttribLocation(program, GFX_TANGENT, "in_tangent");
   *
   * GL_ERROR("bind attrib locations"); */

  glLinkProgram(program);

  GL_PROGRAM_ERROR(program);
  GL_ERROR("link shader program");

  glUseProgram(program);

  GL_ERROR("use shader program");

  /* cleanup */
  glDetachShader(program, vert);
  glDetachShader(program, frag);

  glDeleteShader(vert);
  glDeleteShader(frag);

  shader->loc.projectionMatrix = glGetUniformLocation(program, "projectionMatrix");
  shader->loc.invProjectionMatrix = glGetUniformLocation(program, "invProjectMatrix");
  shader->loc.modelviewMatrix = glGetUniformLocation(program, "modelviewMatrix");
  shader->loc.normalMatrix = glGetUniformLocation(program, "normalMatrix");

  shader->loc.texture0 = glGetUniformLocation(program, "texture0");
  shader->loc.texture1 = glGetUniformLocation(program, "texture1");
  shader->loc.texture2 = glGetUniformLocation(program, "texture2");
  shader->loc.texture3 = glGetUniformLocation(program, "texture3");

  shader->loc.timer = glGetUniformLocation(program, "timer");

  /* uniform blocks for UBO's (Uniform Buffer Objects) */
  shader->loc.matricesBlockIndex = glGetUniformBlockIndex(program, "StaticMatrices");

  if (shader->loc.matricesBlockIndex != GL_INVALID_INDEX) {
    trace("shader requires static matrices, binding UBO\n");
    glUniformBlockBinding(program, shader->loc.matricesBlockIndex, GFX_UBO_LAYER);
  }

  shader->id = program;
}

void gfxDestroyShader(struct gfxShaderProgram *shader) {
  glUseProgram(0);
  glDeleteProgram(shader->id);

  GL_ERROR("delete shader program");
}

void gfxShaderPrintUboLayout(const struct gfxShaderProgram *shader, const char *blockname) {
  struct gfxUboLayout layout;

  gfxShaderUboLayout(shader, &layout, blockname);

  trace("block name: %s\n", layout.name);
  trace("block size: %zu bytes\n", layout.size);
  for (size_t i = 0; i < layout.nelem; ++i) {
    struct gfxUniformInfo *info = &layout.uniforms[i];
    trace("    - %s\n", info->name);
    trace("        type:          %s\n", gfxGlslTypeString(info->type));
    trace("        size:          %d\n", info->size);
    trace("        offset:        %d\n", info->offset);
    trace("        array_stride:  %d\n", info->arrayStride);
    trace("        matrix_stride: %d\n", info->matrixStride);
  }
  gfxShaderFreeUboLayout(&layout);
}

static int gfxUboInfoCompare(const void *p1, const void *p2) {
  GLint a = ((const struct gfxUniformInfo *)p1)->offset;
  GLint b = ((const struct gfxUniformInfo *)p2)->offset;

  return (a < b) ? -1 : (a > b) ? 1
                                : 0;
}

/* fills the layout struct with information about the UBO, return -1 on
 * error, 0 on success. You can reuse gfxUboLayout structs after you've
 * called gfxShaderFreeUboLayout on them. */
int gfxShaderUboLayout(const struct gfxShaderProgram *shader,
                       struct gfxUboLayout *layout, const char *blockname) {
  memset(layout, 0x0, sizeof(*layout));
  memcpy(layout->name, blockname, strlen(blockname));

  /* get the block index */
  GLuint blockIndex = glGetUniformBlockIndex(shader->id, blockname);
  if (blockIndex == GL_INVALID_INDEX) {
    trace("block %s does not exist in shader (id: %u)\n", blockname, shader->id);
    return -1;
  }

  /* get the size of UBO */
  glGetActiveUniformBlockiv(shader->id, blockIndex,
                            GL_UNIFORM_BLOCK_DATA_SIZE, (GLint *)&(layout->size));

  /* get the number of active uniforms of the UBO */
  glGetActiveUniformBlockiv(shader->id, blockIndex,
                            GL_UNIFORM_BLOCK_ACTIVE_UNIFORMS, (GLint *)&(layout->nelem));

  /* get the index each unifom inside the UBO */
  GLuint *uids = zmalloc(layout->nelem * sizeof(GLint));
  glGetActiveUniformBlockiv(shader->id, blockIndex,
                            GL_UNIFORM_BLOCK_ACTIVE_UNIFORM_INDICES, (GLint *)uids);

  /* allocate the array of info structs for all uniforms */
  layout->uniforms = zmalloc(layout->nelem * sizeof(*layout->uniforms));

  /* loop over all the uniforms in the UBO */
  for (size_t i = 0; i < layout->nelem; ++i) {
    GLint type, offset, nelem, arrayStride, matrixStride;

    glGetActiveUniformsiv(shader->id, 1, &uids[i], GL_UNIFORM_TYPE, &type);
    glGetActiveUniformsiv(shader->id, 1, &uids[i], GL_UNIFORM_OFFSET, &offset);
    glGetActiveUniformsiv(shader->id, 1, &uids[i], GL_UNIFORM_ARRAY_STRIDE, &arrayStride);
    glGetActiveUniformsiv(shader->id, 1, &uids[i], GL_UNIFORM_MATRIX_STRIDE, &matrixStride);

    /* this function retrieves array length, not the actual size. For
         * non-array types, this will always be one */
    glGetActiveUniformsiv(shader->id, 1, &uids[i], GL_UNIFORM_SIZE, &nelem);

    int size = 0;
    if (arrayStride > 0) {
      /* the uniform is an array */
      size = arrayStride * nelem;
    } else if (matrixStride > 0) {
      /* the uniform is a matrix */
      switch (type) {
      case GL_FLOAT_MAT2:
      case GL_FLOAT_MAT2x3:
      case GL_FLOAT_MAT2x4:
      case GL_DOUBLE_MAT2:
        size = 2 * matrixStride;
        break;
      case GL_FLOAT_MAT3:
      case GL_FLOAT_MAT3x2:
      case GL_FLOAT_MAT3x4:
      case GL_DOUBLE_MAT3:
        size = 3 * matrixStride;
        break;
      case GL_FLOAT_MAT4:
      case GL_FLOAT_MAT4x2:
      case GL_FLOAT_MAT4x3:
      case GL_DOUBLE_MAT4:
        size = 4 * matrixStride;
        break;
      }
    } else {
      size = (int)gfxGlslTypeSize(type);
    }

    layout->uniforms[i] = (struct gfxUniformInfo){
        .type = type,
        .offset = offset,
        .size = size,
        .arrayStride = arrayStride,
        .matrixStride = matrixStride};
    glGetActiveUniformName(shader->id, uids[i], GFX_SHD_IDENT_SIZE_MAX,
                           NULL, layout->uniforms[i].name);
  }

  /* sort the uniforms based on offset, some drivers return them in random
     * order */
  qsort(layout->uniforms, layout->nelem,
        sizeof(layout->uniforms[0]), gfxUboInfoCompare);

  zfree(uids);

  return 0;
}

void gfxShaderFreeUboLayout(struct gfxUboLayout *layout) {
  zfree(layout->uniforms);
  layout->uniforms = NULL;
}

static int gfxCheckShader(GLuint shader) {
  GLint compiled = 0, length;
  glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);

  if (compiled == GL_FALSE) {
    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length);

    if (length > 0) {
      char *log = zmalloc((size_t)length);
      glGetShaderInfoLog(shader, length, &length, log);

      trace("[ERROR] could not compile shader correctly: %s\n", log);

      zfree(log);
    }

    return 0;
  }

  return 1;
}

static int gfxCheckShaderProgram(GLuint program) {
  GLint linked = 0, length = 0;
  glGetProgramiv(program, GL_LINK_STATUS, &linked);

  if (linked == GL_FALSE) {
    glGetProgramiv(program, GL_INFO_LOG_LENGTH, &length);

    if (length > 0) {
      /* length includes the NULL character */
      char *log = zmalloc((size_t)length);
      glGetProgramInfoLog(program, length, &length, log);

      trace("[ERROR] could not link correctly: %s\n", log);

      zfree(log);
    }

    return 0;
  }

  return 1;
}

/* returns the size in bytes of the GLSL type */
static size_t gfxGlslTypeSize(GLint type) {
  switch (type) {
  case GL_FLOAT: return sizeof(GLfloat);
  case GL_FLOAT_VEC2: return sizeof(GLfloat) * 2;
  case GL_FLOAT_VEC3: return sizeof(GLfloat) * 3;
  case GL_FLOAT_VEC4: return sizeof(GLfloat) * 4;

  case GL_DOUBLE: return sizeof(GLdouble);
  case GL_DOUBLE_VEC2: return sizeof(GLdouble) * 2;
  case GL_DOUBLE_VEC3: return sizeof(GLdouble) * 3;
  case GL_DOUBLE_VEC4: return sizeof(GLdouble) * 4;

  case GL_SAMPLER_1D: return sizeof(GLint);
  case GL_SAMPLER_2D: return sizeof(GLint);
  case GL_SAMPLER_3D: return sizeof(GLint);
  case GL_SAMPLER_CUBE: return sizeof(GLint);
  case GL_SAMPLER_1D_SHADOW: return sizeof(GLint);
  case GL_SAMPLER_2D_SHADOW: return sizeof(GLint);
  case GL_SAMPLER_1D_ARRAY: return sizeof(GLint);
  case GL_SAMPLER_2D_ARRAY: return sizeof(GLint);
  case GL_SAMPLER_1D_ARRAY_SHADOW: return sizeof(GLint);
  case GL_SAMPLER_2D_ARRAY_SHADOW: return sizeof(GLint);
  case GL_SAMPLER_2D_MULTISAMPLE: return sizeof(GLint);
  case GL_SAMPLER_2D_MULTISAMPLE_ARRAY: return sizeof(GLint);
  case GL_SAMPLER_CUBE_SHADOW: return sizeof(GLint);
  case GL_SAMPLER_BUFFER: return sizeof(GLint);
  case GL_SAMPLER_2D_RECT: return sizeof(GLint);
  case GL_SAMPLER_2D_RECT_SHADOW: return sizeof(GLint);
  case GL_INT_SAMPLER_1D: return sizeof(GLint);
  case GL_INT_SAMPLER_2D: return sizeof(GLint);
  case GL_INT_SAMPLER_3D: return sizeof(GLint);
  case GL_INT_SAMPLER_CUBE: return sizeof(GLint);
  case GL_INT_SAMPLER_1D_ARRAY: return sizeof(GLint);
  case GL_INT_SAMPLER_2D_ARRAY: return sizeof(GLint);
  case GL_INT_SAMPLER_2D_MULTISAMPLE: return sizeof(GLint);
  case GL_INT_SAMPLER_2D_MULTISAMPLE_ARRAY: return sizeof(GLint);
  case GL_INT_SAMPLER_BUFFER: return sizeof(GLint);
  case GL_INT_SAMPLER_2D_RECT: return sizeof(GLint);
  case GL_UNSIGNED_INT_SAMPLER_1D: return sizeof(GLint);
  case GL_UNSIGNED_INT_SAMPLER_2D: return sizeof(GLint);
  case GL_UNSIGNED_INT_SAMPLER_3D: return sizeof(GLint);
  case GL_UNSIGNED_INT_SAMPLER_CUBE: return sizeof(GLint);
  case GL_UNSIGNED_INT_SAMPLER_1D_ARRAY: return sizeof(GLint);
  case GL_UNSIGNED_INT_SAMPLER_2D_ARRAY: return sizeof(GLint);
  case GL_UNSIGNED_INT_SAMPLER_2D_MULTISAMPLE: return sizeof(GLint);
  case GL_UNSIGNED_INT_SAMPLER_2D_MULTISAMPLE_ARRAY: return sizeof(GLint);
  case GL_UNSIGNED_INT_SAMPLER_BUFFER: return sizeof(GLint);
  case GL_UNSIGNED_INT_SAMPLER_2D_RECT: return sizeof(GLint);
  case GL_BOOL: return sizeof(GLint);
  case GL_INT: return sizeof(GLint);
  case GL_BOOL_VEC2: return sizeof(GLint) * 2;
  case GL_INT_VEC2: return sizeof(GLint) * 2;
  case GL_BOOL_VEC3: return sizeof(GLint) * 3;
  case GL_INT_VEC3: return sizeof(GLint) * 3;
  case GL_BOOL_VEC4: return sizeof(GLint) * 4;
  case GL_INT_VEC4: return sizeof(GLint) * 4;

  case GL_UNSIGNED_INT: return sizeof(GLint);
  case GL_UNSIGNED_INT_VEC2: return sizeof(GLint) * 2;
  case GL_UNSIGNED_INT_VEC3: return sizeof(GLint) * 2;
  case GL_UNSIGNED_INT_VEC4: return sizeof(GLint) * 2;

  case GL_FLOAT_MAT2: return sizeof(GLfloat) * 4;
  case GL_FLOAT_MAT3: return sizeof(GLfloat) * 9;
  case GL_FLOAT_MAT4: return sizeof(GLfloat) * 16;
  case GL_FLOAT_MAT2x3: return sizeof(GLfloat) * 6;
  case GL_FLOAT_MAT2x4: return sizeof(GLfloat) * 8;
  case GL_FLOAT_MAT3x2: return sizeof(GLfloat) * 6;
  case GL_FLOAT_MAT3x4: return sizeof(GLfloat) * 12;
  case GL_FLOAT_MAT4x2: return sizeof(GLfloat) * 8;
  case GL_FLOAT_MAT4x3: return sizeof(GLfloat) * 12;
  case GL_DOUBLE_MAT2: return sizeof(GLdouble) * 4;
  case GL_DOUBLE_MAT3: return sizeof(GLdouble) * 9;
  case GL_DOUBLE_MAT4: return sizeof(GLdouble) * 16;
  default:
    trace("warning, did not recognize GL type %d, returning 0\n", type);
    return 0;
  }
}

/* returns a string that represents the GLSL type */
static const char *gfxGlslTypeString(GLint type) {
  switch (type) {
  case GL_FLOAT: return "GL_FLOAT";
  case GL_FLOAT_VEC2: return "GL_FLOAT_VEC2";
  case GL_FLOAT_VEC3: return "GL_FLOAT_VEC3";
  case GL_FLOAT_VEC4: return "GL_FLOAT_VEC4";
  case GL_DOUBLE: return "GL_DOUBLE";
  case GL_DOUBLE_VEC2: return "GL_DOUBLE_VEC2";
  case GL_DOUBLE_VEC3: return "GL_DOUBLE_VEC3";
  case GL_DOUBLE_VEC4: return "GL_DOUBLE_VEC4";
  case GL_SAMPLER_1D: return "GL_SAMPLER_1D";
  case GL_SAMPLER_2D: return "GL_SAMPLER_2D";
  case GL_SAMPLER_3D: return "GL_SAMPLER_3D";
  case GL_SAMPLER_CUBE: return "GL_SAMPLER_CUBE";
  case GL_SAMPLER_1D_SHADOW: return "GL_SAMPLER_1D_SHADOW";
  case GL_SAMPLER_2D_SHADOW: return "GL_SAMPLER_2D_SHADOW";
  case GL_SAMPLER_1D_ARRAY: return "GL_SAMPLER_1D_ARRAY";
  case GL_SAMPLER_2D_ARRAY: return "GL_SAMPLER_2D_ARRAY";
  case GL_SAMPLER_1D_ARRAY_SHADOW: return "GL_SAMPLER_1D_ARRAY_SHADOW";
  case GL_SAMPLER_2D_ARRAY_SHADOW: return "GL_SAMPLER_2D_ARRAY_SHADOW";
  case GL_SAMPLER_2D_MULTISAMPLE: return "GL_SAMPLER_2D_MULTISAMPLE";
  case GL_SAMPLER_2D_MULTISAMPLE_ARRAY: return "GL_SAMPLER_2D_MULTISAMPLE_ARRAY";
  case GL_SAMPLER_CUBE_SHADOW: return "GL_SAMPLER_CUBE_SHADOW";
  case GL_SAMPLER_BUFFER: return "GL_SAMPLER_BUFFER";
  case GL_SAMPLER_2D_RECT: return "GL_SAMPLER_2D_RECT";
  case GL_SAMPLER_2D_RECT_SHADOW: return "GL_SAMPLER_2D_RECT_SHADOW";
  case GL_INT_SAMPLER_1D: return "GL_INT_SAMPLER_1D";
  case GL_INT_SAMPLER_2D: return "GL_INT_SAMPLER_2D";
  case GL_INT_SAMPLER_3D: return "GL_INT_SAMPLER_3D";
  case GL_INT_SAMPLER_CUBE: return "GL_INT_SAMPLER_CUBE";
  case GL_INT_SAMPLER_1D_ARRAY: return "GL_INT_SAMPLER_1D_ARRAY";
  case GL_INT_SAMPLER_2D_ARRAY: return "GL_INT_SAMPLER_2D_ARRAY";
  case GL_INT_SAMPLER_2D_MULTISAMPLE: return "GL_INT_SAMPLER_2D_MULTISAMPLE";
  case GL_INT_SAMPLER_2D_MULTISAMPLE_ARRAY: return "GL_INT_SAMPLER_2D_MULTISAMPLE_ARRAY";
  case GL_INT_SAMPLER_BUFFER: return "GL_INT_SAMPLER_BUFFER";
  case GL_INT_SAMPLER_2D_RECT: return "GL_INT_SAMPLER_2D_RECT";
  case GL_UNSIGNED_INT_SAMPLER_1D: return "GL_UNSIGNED_INT_SAMPLER_1D";
  case GL_UNSIGNED_INT_SAMPLER_2D: return "GL_UNSIGNED_INT_SAMPLER_2D";
  case GL_UNSIGNED_INT_SAMPLER_3D: return "GL_UNSIGNED_INT_SAMPLER_3D";
  case GL_UNSIGNED_INT_SAMPLER_CUBE: return "GL_UNSIGNED_INT_SAMPLER_CUBE";
  case GL_UNSIGNED_INT_SAMPLER_1D_ARRAY: return "GL_UNSIGNED_INT_SAMPLER_1D_ARRAY";
  case GL_UNSIGNED_INT_SAMPLER_2D_ARRAY: return "GL_UNSIGNED_INT_SAMPLER_2D_ARRAY";
  case GL_UNSIGNED_INT_SAMPLER_2D_MULTISAMPLE: return "GL_UNSIGNED_INT_SAMPLER_2D_MULTISAMPLE";
  case GL_UNSIGNED_INT_SAMPLER_2D_MULTISAMPLE_ARRAY: return "GL_UNSIGNED_INT_SAMPLER_2D_MULTISAMPLE_ARRAY";
  case GL_UNSIGNED_INT_SAMPLER_BUFFER: return "GL_UNSIGNED_INT_SAMPLER_BUFFER";
  case GL_UNSIGNED_INT_SAMPLER_2D_RECT: return "GL_UNSIGNED_INT_SAMPLER_2D_RECT";
  case GL_BOOL: return "GL_BOOL";
  case GL_INT: return "GL_INT";
  case GL_BOOL_VEC2: return "GL_BOOL_VEC2";
  case GL_INT_VEC2: return "GL_INT_VEC2";
  case GL_BOOL_VEC3: return "GL_BOOL_VEC3";
  case GL_INT_VEC3: return "GL_INT_VEC3";
  case GL_BOOL_VEC4: return "GL_BOOL_VEC4";
  case GL_INT_VEC4: return "GL_INT_VEC4";
  case GL_UNSIGNED_INT: return "GL_UNSIGNED_INT";
  case GL_UNSIGNED_INT_VEC2: return "GL_UNSIGNED_INT_VEC2";
  case GL_UNSIGNED_INT_VEC3: return "GL_UNSIGNED_INT_VEC3";
  case GL_UNSIGNED_INT_VEC4: return "GL_UNSIGNED_INT_VEC4";
  case GL_FLOAT_MAT2: return "GL_FLOAT_MAT2";
  case GL_FLOAT_MAT3: return "GL_FLOAT_MAT3";
  case GL_FLOAT_MAT4: return "GL_FLOAT_MAT4";
  case GL_FLOAT_MAT2x3: return "GL_FLOAT_MAT2x3";
  case GL_FLOAT_MAT2x4: return "GL_FLOAT_MAT2x4";
  case GL_FLOAT_MAT3x2: return "GL_FLOAT_MAT3x2";
  case GL_FLOAT_MAT3x4: return "GL_FLOAT_MAT3x4";
  case GL_FLOAT_MAT4x2: return "GL_FLOAT_MAT4x2";
  case GL_FLOAT_MAT4x3: return "GL_FLOAT_MAT4x3";
  case GL_DOUBLE_MAT2: return "GL_DOUBLE_MAT2";
  case GL_DOUBLE_MAT3: return "GL_DOUBLE_MAT3";
  case GL_DOUBLE_MAT4: return "GL_DOUBLE_MAT4";
  default: return "UNKNOWN";
  }
}
