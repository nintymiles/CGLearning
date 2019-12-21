#version 410 core

uniform mat4 uProjMatrix;
uniform mat4 uModelViewMatrix;
uniform mat4 uNormalMatrix;

in vec3 aPosition;
in vec3 aNormal;
in vec3 aTangent;
in vec3 aBinormal;
in vec2 aTexCoord;

out vec2 vTexCoord;
out mat3 vTMat;  // tangent frame matrix
out vec3 vEyePos; // position in eye space

void main() {
    vTexCoord = aTexCoord;
    
    //bump mapping performed in the eye frame
    vec3 normal = normalize(mat3(uNormalMatrix) * aNormal);
    vec3 tangent = normalize(mat3(uNormalMatrix) * aTangent);
    vec3 binormal = normalize(mat3(uNormalMatrix) * aBinormal);
    //vec3 binormal =  cross( normal, tangent );
    
    // tangent frame matrix will map tangent coordinate into eye coordinate
    vTMat = mat3(tangent, binormal, normal);
    vec4 posE = uModelViewMatrix * vec4(aPosition, 1.0);
    vEyePos = posE.xyz;
    gl_Position = uProjMatrix * posE;
}
