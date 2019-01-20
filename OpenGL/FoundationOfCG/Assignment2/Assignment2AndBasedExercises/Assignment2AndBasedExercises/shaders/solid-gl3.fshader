#version 410 core

uniform vec3 uColor;

out vec4 fragColor;

void main() {
  fragColor = vec4(uColor, 1.0);
}
