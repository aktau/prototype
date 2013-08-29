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

precision highp float;

/* uniforms */
uniform sampler2D tex;

/* in */
in vec2 uv;

/* out */
out vec3 fragColor;

void main() {
    fragColor = texture(tex, uv).rgb;
}