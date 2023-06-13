/**
 * This file is part of prototype.
 *
 * (c) 2013 Nicolas Hillegeer <nicolas@hillegeer.com>
 *
 * For the full copyright and license information, please view the LICENSE
 * file that was distributed with the source code.
 */

#include "util.h"

#include "stb_image.h"

static int gfxUploadTexture(const char *image, int srgb);

float g_max_anisotropy = 0.0f;

void gfxInitTexture(void) {
#if defined(GL_EXT_texture_filter_anisotropic) || defined(GL_ARB_texture_filter_anisotropic)
    glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &g_max_anisotropy);
    if (glGetError() != GL_NO_ERROR) {
      g_max_anisotropy = 0.0f;
      trace("anisotropy not supported, disabling");
    }
#endif
}

/**
 * will return a texture id with a nice, trilinearly filtered texture (with anisotropy)
 *
 * Based, among others, on:
 *
 * http://www.arcsynthesis.org/gltut/Texturing/Tut15%20Anisotropy.html (uses sampler config, but same thing)
 * http://gregs-blog.com/2008/01/17/opengl-texture-filter-parameters-explained/
 * http://www.dhpoware.com/demos/gl3HelloWorld.html
 *
 * NOTE: when converting the code for OpenGL 3.3, start using Sampler Objects,
 * they are really handy: http://www.opengl.org/wiki/Sampler_(GLSL). Sampler
 * Objects overwrite any texture state (clamping, repeat, anisotropy, ...),
 * an example:
 * http://www.geeks3d.com/20110908/opengl-3-3-sampler-objects-control-your-texture-units/
 *
 * Samplers:
 * http://www.opengl.org/wiki/GLSL_Sampler#Binding_textures_to_samplers
 */
GLuint gfxLoadTexture(const char *image) {
    GLuint texture;

    /* generate and bind */
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    GL_ERROR("generate texture");

    /* repeat the texture */
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    /* trilinear filtering */
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

    /* anisotropic filtering */
#if defined(GL_EXT_texture_filter_anisotropic) || defined(GL_ARB_texture_filter_anisotropic)
    if (g_max_anisotropy > 0.0f) {
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, g_max_anisotropy);
    }
#endif

    GL_ERROR("set texture parameters");

    /* load and upload the image, then free */
    int success = gfxUploadTexture(image, 0);
    ERROR_HANDLE(success == 0, errno, "error while uploading texture");

    /* generate the mipmaps */
    glGenerateMipmap(GL_TEXTURE_2D);

    GL_ERROR("generate mipmaps");

    trace("successfully loaded %s, anisotropy: %f\n", image, g_max_anisotropy);

    return texture;

error:
    glDeleteTextures(1, &texture);

    return 0;
}

void gfxDestroyTexture(GLuint texture) {
    glDeleteTextures(1, &texture);
}

/**
 * Uploads an image to the currently bound texture,
 * returns 0 for failure, 1 for success
 *
 * in general, the preferred form of upload to the GPU
 * is with UNPACK_ALIGNMENT = 4, and format GL_BGRA8, though
 * GL_RGBA8 is usually also fine: http://www.opengl.org/wiki/Common_Mistakes
 */
static int gfxUploadTexture(const char *image, int srgb) {
    int width, height, components;
    unsigned char *data = NULL;
    GLint intfmt;
    GLenum fmt;

    data = stbi_load(image, &width, &height, &components, 0);
    ERROR_HANDLE(data == NULL, errno, "couldn't load image (perhaps it doesn't exist, or it is corrupt");

    /* be specific about the format we upload */
    if (components == 1) {
        intfmt = GL_R8;
        fmt    = GL_RED;
    }
    else if (components == 2) {
        intfmt = GL_RG8;
        fmt    = GL_RG;
    }
    else if (components == 3) {
        intfmt = (srgb) ? GL_SRGB8 : GL_RGB8;
        fmt    = GL_RGB;
    }
    else {
        intfmt = (srgb) ? GL_SRGB8_ALPHA8 : GL_RGBA8;
        fmt    = GL_RGBA;
    }

    /**
     * if we only wanted to upload a sub-image, we could set the stride
     * glPixelStorei(GL_UNPACK_ROW_LENGTH, stride);
     * though we shouldn't forget to set it back to 0 later
     *
     * ref: http://stackoverflow.com/questions/205522/opengl-subtexturing
     */

    /* the default value is 4, but it doesn't hurt to set it to make sure */
    glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
    glTexImage2D(GL_TEXTURE_2D, 0, intfmt, width, height, 0, fmt, GL_UNSIGNED_BYTE, data);
    GL_ERROR("upload image data");

    stbi_image_free(data);

    return 1;

error:
    stbi_image_free(data);

    return 0;
}
