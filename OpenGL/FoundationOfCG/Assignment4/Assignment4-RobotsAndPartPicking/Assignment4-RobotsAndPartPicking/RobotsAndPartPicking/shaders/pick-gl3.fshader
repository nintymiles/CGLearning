#version 410 core

uniform vec3 uIdColor;

out vec4 fragColor;

void main() {
  fragColor = vec4(uIdColor, 1.0);
}
