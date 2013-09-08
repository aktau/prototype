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
uniform mat4 modelviewMatrix;
uniform float timer;

/* in */
layout(location = 0) in vec3 in_position;
layout(location = 3) in vec3 in_color;

/* out */
out vec3 vertColor;

void main() {
    float sint = sin(timer);
    float cost = cos(timer);

    vertColor = in_color * vec3(sint * sint, cost * cost, 1.0);

    // vec3 druggedColor = mix(vec3(sin(timer), cos(timer), 1.0), vertColor, 1.0 + sin(timer) * 0.5);

    gl_Position = projectionMatrix * modelviewMatrix * vec4(in_position, 1.0);
}