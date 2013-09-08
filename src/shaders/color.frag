/**
 * This file is part of prototype.
 *
 * (c) 2013 Nicolas Hillegeer <nicolas@hillegeer.com>
 *
 * For the full copyright and license information, please view the LICENSE
 * file that was distributed with the source code.
 */

#version 150

/* in */
in vec3 vertColor;

/* out */
out vec4 fragColor;

void main() {
    fragColor = vec4(vertColor, 1.0);
}