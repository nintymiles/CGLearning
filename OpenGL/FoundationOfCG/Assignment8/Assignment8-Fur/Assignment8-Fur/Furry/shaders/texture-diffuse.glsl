#version 410 core

uniform vec3 uColor;
uniform sampler2D uTexColor;

uniform vec3 uLight, uLight2;

in vec3 vNormal;
in vec3 vPosition;

in vec2 vTexCoord;

out vec4 fragColor;

void main() {
vec3 tolight = normalize(uLight - vPosition);
vec3 tolight2 = normalize(uLight2 - vPosition);
vec3 normal = normalize(vNormal);

float diffuse = max(0.0, dot(normal, tolight));
diffuse += max(0.0, dot(normal, tolight2));

vec4 color = texture(uTexColor, vTexCoord);

vec4 intensity = color * diffuse;

fragColor = intensity;
}
