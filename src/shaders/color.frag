#version 150
#extension GL_ARB_explicit_attrib_location : enable

in vec3 vertColor;

out vec4 fragColor;

void main() {
    fragColor = vec4(vertColor, 1.0);
}