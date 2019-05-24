uniform mat4 uProjMatrix;
uniform mat4 uModelViewMatrix;
uniform mat4 uNormalMatrix;

attribute vec3 aPosition;
attribute vec3 aNormal;
attribute vec3 aTangent;
attribute vec3 aBinormal;
attribute vec2 aTexCoord;

varying vec2 vTexCoord;
varying mat3 vNTMat;  // normal matrix * tangent frame matrix
varying vec3 vEyePos; // position in eye space


void main() {
  vTexCoord = aTexCoord;
  vNTMat = mat3(uNormalMatrix[0].xyz, uNormalMatrix[1].xyz, uNormalMatrix[2].xyz) * mat3(aTangent, aBinormal, aNormal);
  vec4 posE = uModelViewMatrix * vec4(aPosition, 1.0);
  vEyePos = posE.xyz;
  gl_Position = uProjMatrix * posE;
}