#version 150

in vec3 in_position;
in vec3 in_normal;

vec3 lightPos = vec3(0.0, 1.0, -2.0);
out vec3 normal;
out vec3 lightDir;

void main() {
    vec4 worldPos = vec4(in_position, 1.0); gl_Position = worldPos;

    normal   = normalize(in_normal);
    lightDir = normalize(lightPos - worldPos.xyz);
}