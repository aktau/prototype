/**
 * This file is part of prototype.
 *
 * (c) 2013 Nicolas Hillegeer <nicolas@hillegeer.com>
 *
 * For the full copyright and license information, please view the LICENSE
 * file that was distributed with the source code.
 *
 * layout(location = 0) = position
 * layout(location = 1) = normal
 * layout(location = 2) = texcoord
 * layout(location = 3) = color
 * layout(location = 4) = tangent
 */

#version 150
#extension GL_ARB_explicit_attrib_location : enable

/* uniform buffers */
layout(std140) uniform StaticMatrices {
    mat4 projectionMatrix;
    // mat3 normalMatrix;
    // vec4 ambient;
    // vec4 diffuse;
    // vec4 specular;
    // vec4 position;
    // vec4 direction;
};

/* uniforms */
// uniform mat4 projection;

/* in */
layout(location = 0) in vec3 in_position;

/* out */
// out vec3 color;

void main() {
    // color = vec3(0.5, 1.0, 1.0);
    gl_Position = projectionMatrix * vec4(in_position, 1.0);
}
