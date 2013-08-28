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