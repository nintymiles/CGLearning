#version 130

uniform mat4 uProjMatrix;
uniform mat4 uModelViewMatrix;
uniform mat4 uNormalMatrix;

in vec3 aPosition;
in vec3 aNormal;
in vec3 aTangent;
in vec3 aBinormal;
in vec2 aTexCoord;

out vec2 vTexCoord;
out mat3 vNTMat;  // normal matrix * tangent frame matrix
out vec3 vEyePos; // position in eye space

void main() {
  vTexCoord = aTexCoord;
  vNTMat = mat3(uNormalMatrix) * mat3(aTangent, aBinormal, aNormal);
  vec4 posE = uModelViewMatrix * vec4(aPosition, 1.0);
  vEyePos = posE.xyz;
  gl_Position = uProjMatrix * posE;
}