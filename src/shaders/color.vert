#version 150

// uniform mat4 viewMatrix, projMatrix;

in vec4 in_position;
in vec3 in_color;

out vec3 vertColor;

void main() {
    vertColor   = in_color;
    gl_Position = in_position; /* projMatrix * viewMatrix * position */
}