#version 410 core

uniform vec3 uColor;
uniform sampler2D uTexColor;

in vec2 vTexCoord;

out vec4 fragColor;

void main() {
    vec4 color = texture(uTexColor, vTexCoord);
fragColor = color;//vec4(0,vTexCoord.x,vTexCoord.y,1);
}
