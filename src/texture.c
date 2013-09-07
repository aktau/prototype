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

static int gfxUploadTexture(const char *image);

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
    GLfloat anisotropy = 0.0f;
    GLuint texture;

    /* get maximum possible anisotropric filtering value */
    glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &anisotropy);

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
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, anisotropy);

    GL_ERROR("set texture parameters");

    /* load and upload the image, then free */
    int success = gfxUploadTexture(image);
    ERROR_HANDLE(success == 0, errno, "error while uploading texture");

    /* generate the mipmaps */
    glGenerateMipmap(GL_TEXTURE_2D);

    GL_ERROR("generate mipmaps");

    trace("successfully loaded %s, anisotropy: %f\n", image, anisotropy);

    return texture;

error:
    glDeleteTextures(1, &texture);

    return 0;
}

/**
 * Uploads an image to the currently bound texture,
 * returns 0 for failure, 1 for success
 */
static int gfxUploadTexture(const char *image) {
    int width, height, components;
    unsigned char *data = NULL;

    data = stbi_load(image, &width, &height, &components, 0);
    ERROR_HANDLE(data == NULL, errno, "couldn't load image (perhaps it doesn't exist, or it is corrupt");

    GLenum format = (components == 3) ? GL_RGB : GL_RGBA;
    ERROR_HANDLE((components != 3 && components != 4), errno, "don't know how to handle %d components in image data\n", components);

    glTexImage2D(GL_TEXTURE_2D, 0, (GLint) format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
    GL_ERROR("upload image data");

    stbi_image_free(data);

    return 1;

error:
    stbi_image_free(data);

    return 0;
}
