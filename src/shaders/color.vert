#version 150

// uniform mat4 viewMatrix, projMatrix;

in vec3 in_position;
in vec3 in_color;

out vec3 vertColor;

void main() {
    vertColor   = in_color;
    gl_Position = vec4(in_position, 1.0); /* projMatrix * viewMatrix * position */
}