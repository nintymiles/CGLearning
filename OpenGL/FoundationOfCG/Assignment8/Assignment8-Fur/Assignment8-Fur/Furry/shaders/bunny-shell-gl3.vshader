#version 410 core

uniform mat4 uProjMatrix;
uniform mat4 uModelViewMatrix;
uniform mat4 uNormalMatrix;

in vec3 aPosition;
in vec3 aNormal;
in vec2 aTexCoord;

out vec3 vNormal;
out vec3 vPosition;
out vec2 vTexCoord;

void main() {
  vNormal = vec3(uNormalMatrix * vec4(aNormal, 0.0));
  vTexCoord = aTexCoord;

  vec4 tPosition = uModelViewMatrix * vec4(aPosition, 1.0);

  vPosition = tPosition.xyz;
  gl_Position = uProjMatrix * tPosition;
}
