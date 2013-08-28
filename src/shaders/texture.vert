#version 150

in vec3 in_position;
in vec2 in_uv;

out vec2 uv;

void main() {
    uv          = in_uv;
    gl_Position = vec4(in_position, 1.0);
}