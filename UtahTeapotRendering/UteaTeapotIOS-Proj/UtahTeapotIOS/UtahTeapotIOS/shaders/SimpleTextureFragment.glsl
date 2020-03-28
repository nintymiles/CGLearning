#version 300 es
precision highp float;

uniform sampler2D samplerObj;

in vec2 vTex;

out vec4 FragColor;

void main() {
    FragColor = texture(samplerObj,vTex);
}
