#version 300 es

in highp vec3    myVertex;
in highp vec3    myNormal;
in mediump vec2  myUV;
in mediump vec4  myBone;

out lowp    vec4    colorDiffuse;

out mediump vec3 position;
out mediump vec3 normal;

uniform highp mat4      uMVMatrix;
uniform highp mat4      uMVPMatrix;
uniform highp mat4      uMMatrix;

uniform highp vec3      vLight0;

uniform lowp vec4       vMaterialDiffuse;
uniform lowp vec3       vMaterialAmbient;
uniform lowp vec4       vMaterialSpecular;

void main(void)
{
    highp vec4 p = vec4(myVertex,1);
    gl_Position = uMVPMatrix * p;
    
    mat3 normalMatrix = mat3(uMVMatrix[0].xyz, uMVMatrix[1].xyz, uMVMatrix[2].xyz);

    highp vec3 eyeNormal = vec3(normalMatrix * myNormal);
    highp vec3 eyePosition = vec3(uMVMatrix * p);

    // brighten the teapot color by 50%
    colorDiffuse = clamp(vMaterialDiffuse * 1.5,
                         vec4(0.0, 0.0, 0.0, 0.0),
                         vec4(1.0, 1.0, 1.0, 1.0));

    normal = eyeNormal;
    position = eyePosition;
}
