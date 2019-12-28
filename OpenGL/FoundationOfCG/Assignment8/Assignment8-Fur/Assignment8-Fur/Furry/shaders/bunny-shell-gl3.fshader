#version 410 core

uniform sampler2D uTexShell;

uniform vec3 uLight;

uniform float uAlphaExponent;

in vec3 vNormal;
in vec3 vPosition;
in vec2 vTexCoord;

out vec4 fragColor;

void main() {
  vec3 normal = normalize(vNormal);
  vec3 toLight = normalize(uLight - vPosition);

  vec3 toP = -normalize(vPosition);

  vec3 h = normalize(toP + toLight);

  float u = dot(normal, toLight);
  float v = dot(normal, toP);
  u = 1.0 - u*u;
  v = pow(1.0 - v*v, 16.0);

  float r = 0.009+ 0.43 * u + 0.25* v;
  float g = 0.009+ 0.13* u + 0.21* v;
  float b = 0.009+ 0.02 * u + 0.21* v;

  float alpha = pow(texture(uTexShell, vTexCoord).r, uAlphaExponent);

  fragColor = vec4(r, g, b, alpha);
}
