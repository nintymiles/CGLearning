#version 300 es

precision highp float;

uniform vec3 uLight, uLight2;
uniform vec4 uColor;

in vec3 vNormal;
in vec3 vPosition;

out vec4 fragColor;

void main() {
  vec3 tolight = normalize(uLight - vPosition);
  vec3 tolight2 = normalize(uLight2 - vPosition);
  vec3 normal = normalize(vNormal);

  float diffuse = max(0.0, dot(normal, tolight));
  diffuse += max(0.0, dot(normal, tolight2));
  vec4 intensity = uColor * diffuse;

  fragColor = vec4(vec3(intensity),uColor.a);
}
