#version 150

precision highp float;

in vec3 normal;
in vec3 lightDir;
out vec4 fragColor;

void main() {
    float lambert = max(dot(normal, lightDir), 0.0); float ambient = 0.2;

    vec3 white = vec3(1.0, 1.0, 1.0);
    vec3 c = white * (lambert + ambient);

    fragColor = clamp(vec4(c, 1.0), 0.0, 1.0);
}